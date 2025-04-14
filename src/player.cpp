#include "player.hpp"
#include "camera.hpp"
#include "engine/entity.hpp"
#include "engine/core.h"
#include "border.hpp"
#include "healthManager.hpp"
#include "playerBullet.hpp"
#include "afterimage.hpp"
#include "bars.hpp"
#include "particle.hpp"
#include <cmath>
#include <cstdlib>
#include <raylib.h>
#include <string>

#define max(a, b) (a < b ? b : a)
#define min(a, b) (a < b ? a : b)

int Player::upKey = KEY_W;
int Player::downKey = KEY_S;
int Player::leftKey = KEY_A;
int Player::rightKey = KEY_D;

int Player::shootKey = 0;
int Player::shootKeyMouse = MOUSE_BUTTON_LEFT;

int Player::dashKey = KEY_SPACE;
float Player::dashTime = .4;
float Player::dashSpeed = 2500;
float Player::dashControl = 2;

unsigned Player::maxDashCount = 3;

const float Player::defaultSpeed = 4000;
const float Player::defaultFriction = 58;

float Player::dashCooldown = 1.5;

float Player::particleSpawnTime = 1.0f / 30.0f;

const float distance = 50;

float Player::hitboxRadius = 25;

#define barDimensions (Vector2){10, 100}

Vector2 Player::getInput() {
  return Player::getInput(Player::upKey, Player::downKey, Player::leftKey, Player::rightKey);
}

Vector2 Player::getInput(int u, int d, int l, int r) {
  Vector2 out = (Vector2){(float)IsKeyDown(r) - (float)IsKeyDown(l), (float)IsKeyDown(d) - (float)IsKeyDown(u)};
  return Vector2Normalize(out);
}

void Player::Render() {
  //get the mouse position (in cartesian)
  //draw our triangle
  DrawTriangle(
    Position,
    Vector2Add(Position, (Vector2){cosf(Rotation + 4 * M_PI / 3) * distance, -sinf(Rotation + 4 * M_PI / 3) * distance}),
    Vector2Add(Position, (Vector2){cosf(Rotation) * distance, -sinf(Rotation) * distance}),
    YELLOW
  );
  DrawTriangle(
    Position,
    Vector2Add(Position, (Vector2){cosf(Rotation) * distance, -sinf(Rotation) * distance}),
    Vector2Add(Position, (Vector2){cosf(Rotation + 2 * M_PI / 3) * distance, -sinf(Rotation + 2 * M_PI / 3) * distance}),
    YELLOW
  );

  // we draw them darn sqrs
  const float height = 5;
  const Vector2 dems = {dashCooldownBar->Dimensions.x, height};
  // maxDashCount is the amount we draw
  float offsetY = dashCooldownBar->Dimensions.y / maxDashCount;

  for(int i = 0; i < maxDashCount; i++) {
    DrawRectangleV((Vector2){dashCooldownBar->Position.x, dashCooldownBar->Position.y + (i * offsetY)}, 
                   dems, dashCooldownBar->EmptyCol);
  }

  DrawCircleV(Position, 5, WHITE);
}

void Player::Process(float delta) {
  lifetime += delta;
  Vector2 inputDirection = getInput();
  Velocity = Vector2Add(Vector2Scale(inputDirection, delta * Speed), Velocity);
  Position = Vector2Add(Position, Vector2Scale(Velocity, delta));
  Velocity = Vector2Scale(Velocity, delta * Friction);

  Border::wrapEntity(this);

  if(dashing || Vector2LengthSqr(inputDirection))
    if(fmodf(lifetime - particleSpawnTime, particleSpawnTime) <= 1.0 / 60.0f)
      getRoot()->addChild(new Particle(Position, Vector2Scale(Vector2Add(Velocity, Vector2Scale(inputDirection, Speed * delta)), -1)));

  timeSinceDash += delta;
  if(dashing) {
    Velocity = dashDirection;
    //i wanna do something funny.
    //that funny is changing the dashDirection.
    //based on the playerInput.
    dashDirection = Vector2Scale(Vector2Normalize(Vector2Add(dashDirection, Vector2Scale(inputDirection, (delta / dashTime) * dashSpeed * dashControl))), dashSpeed);

    if(fmodf(timeSinceDash, .1) < 1.0f / 120.0f)
      getRoot()->addChild(new Afterimage(Position, Rotation));

    if(timeSinceDash > dashTime)
      dashing = false;
  }
  if(IsKeyPressed(dashKey) && !dashing && dashProgress > 1) {
    dashing = true;
    dashProgress--;
    dashDirection = Vector2Length(inputDirection) > 0 ? Vector2Scale(inputDirection, dashSpeed) : Vector2Scale(Vector2Normalize(Velocity), dashSpeed);
    timeSinceDash = 0;
  } else if(dashProgress <= maxDashCount)
    dashProgress += delta / dashCooldown;
  if((IsKeyPressed(shootKey) || IsMouseButtonPressed(shootKeyMouse)) && !dashing)
    SpawnBullet();
  if(healthManager->isDead())
    killDefered();
  manageBars();
  manageRotation();
}

void Player::SpawnBullet() {
  const float offsetAhead = 30;
  Bullet* bul = new PlayerBullet(Vector2Add(Position, (Vector2){cosf(Rotation) * offsetAhead, -sinf(Rotation) * offsetAhead}), Rotation);
  getRoot()->addChild(bul);
}

void Player::manageBars() {
  manageBar(dashCooldownBar, 1, dashProgress / maxDashCount, true);
}

void Player::manageBar(Bar* b, int index, float p, bool shouldRender) {
  //so basically, math :thumbsup:
  //bool for verticle b->growVert
  float offsetX = /*this one is either constant or index dependant depending on growVert*/ !b->ShrinkY ?
    -b->Dimensions.x / 2.0f : distance + b->Dimensions.x * index;
  float offsetY = /*just the opposite of ^*/ b->ShrinkY ?
    -b->Dimensions.y / 2.0f : distance + b->Dimensions.y * index;
  Vector2 finalPosition = {Position.x + offsetX, Position.y + offsetY};
  b->Position = finalPosition;
  b->ShouldRender = shouldRender;
  b->Progress = p;
}

void Player::manageRotation() {
  Vector2 mousePos = Vector2Scale(Vector2Subtract(GetMousePosition(), cam->Camera.offset), 1.0f / cam->Camera.zoom);
  //then we also have to globalize the mouse position good thing we have a cam field
  mousePos = Vector2Add(mousePos, cam->Camera.target);
  Rotation = atan2f(-(mousePos.y - Position.y), mousePos.x - Position.x); // then it's as simple as b - a
}

Player::Player(const std::string& name, Vector2 position, CameraEntity* camera) : Entity2D(name, position), cam(camera) {

  healthManager = new HealthManager(10, BarManager(&Position, distance, Bar(Position, (Vector2){barDimensions.y, barDimensions.x}, RED, DARKGRAY, false)));
  addChild(healthManager);

  Velocity = (Vector2){0,0};
  Speed = defaultSpeed;
  Friction = defaultFriction;

  dashDirection = Vector2Zero();
  dashing = false;
  dashProgress = 0;
  timeSinceDash = 0;

  dashCooldownBar = new Bar(Position, barDimensions, YELLOW, (Color){10, 10, 10, 255}, true);
  addChild(dashCooldownBar);
  Enemy::setPlayer();
}

Player::~Player() {
  Enemy::setPlayer(nullptr);
  cam->Follow = nullptr;
}

void Player::Init() {
  Enemy::setPlayer();
};

void Player::setCam(CameraEntity* camra) {
  cam = camra;
}

float Player::getLifetime() {
  return lifetime;
}

HealthManager* Player::getHealthManager() {
  return healthManager;
}

bool Player::getDashing() {
  return dashing;
}

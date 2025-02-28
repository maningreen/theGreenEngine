#include "player.hpp"
#include "camera.hpp"
#include "entity.hpp"
#include "core.h"
#include "border.hpp"
#include "playerBullet.hpp"
#include "afterimage.hpp"
#include "bars.hpp"
#include "particle.hpp"
#include "enemyBullet.hpp"
#include <cmath>
#include <cstdlib>
#include "include.h"
#include <string>
#include <iostream>

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

const float Player::defaultSpeed = 4000;
const float Player::defaultFriction = 58;

float Player::dashCooldown = .7;

float Player::particleSpawnTime = 1.0f / 10.0f;

const float distance = 50;

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

  DrawCircleV(Position, 5, WHITE);
}

void Player::Process(float delta) {
  lifetime += delta;
  Vector2 inputDirection = getInput();
  Velocity = Vector2Add(Vector2Scale(inputDirection, delta * Speed), Velocity);
  Position = Vector2Add(Position, Vector2Scale(Velocity, delta));
  Velocity = Vector2Scale(Velocity, delta * Friction);
  if(dashing || Vector2LengthSqr(inputDirection))
    if(fmodf(lifetime - particleSpawnTime, particleSpawnTime) <= 1.0 / 60.0f)
      addChild(new Particle(Position, Vector2Scale(Vector2Add(Velocity, Vector2Scale(inputDirection, Speed * delta)), -1)));
  wrapPosition();
  timeSinceDash += delta;
  if(dashing) {
    Velocity = dashDirection;
    //i wanna do something funny.
    //that funny is changing the dashDirection.
    //based on the playerInput.
    if(Vector2LengthSqr(inputDirection) != 0)
      dashDirection = Vector2Scale(Vector2Normalize(Vector2Add(dashDirection, Vector2Scale(inputDirection, (delta / dashTime) * dashSpeed * dashControl))), dashSpeed);
    if(fmodf(timeSinceDash, .1) < 1.0f / 120.0f)
      addChild(new Afterimage(Position, Rotation));
    if(timeSinceDash > dashTime)
      dashing = false;
  }
  if(IsKeyPressed(dashKey) && !dashing && timeSinceDash >= dashCooldown) {
    dashing = true;
    dashDirection = Vector2Length(inputDirection) > 0 ? Vector2Scale(inputDirection, dashSpeed) : Vector2Scale(Vector2Normalize(Velocity), dashSpeed);
    timeSinceDash = 0;
  }
  if(IsKeyPressed(shootKey) || IsMouseButtonPressed(shootKeyMouse))
    SpawnBullet();
  manageBars();
  manageRotation();
}

void Player::SpawnBullet() {
  const float offsetAhead = 30;
  Bullet* bul = new PlayerBullet(Vector2Add(Position, (Vector2){cosf(Rotation) * offsetAhead, -sinf(Rotation) * offsetAhead}), Rotation);
  addChild(bul);
}

void Player::wrapPosition() {
  bool out = false;
  if(Position.x < -Border::Length)
    Position.x += Border::Length * 2;
  else if(Position.x > Border::Length)
    Position.x -= Border::Length * 2;
  if(Position.y < -Border::Length)
    Position.y += Border::Length * 2;
  else if(Position.y > Border::Length)
    Position.y -= Border::Length * 2;
}

void Player::manageBars() {
  manageBar(dashCooldownBar, 1, timeSinceDash / dashCooldown, timeSinceDash <= dashCooldown);
  manageBar(dashBar, 3, timeSinceDash / dashTime, timeSinceDash <= dashTime);
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
  Velocity = (Vector2){0,0};
  Speed = defaultSpeed;
  Friction = defaultFriction;
  dashing = false;
  dashDirection = Vector2Zero();
  dashCooldownBar = new Bar(Position, barDimensions, YELLOW, (Color){10, 10, 10, 255}, true);
  addChild(dashCooldownBar);
  timeSinceDash = 0;
  dashBar = new Bar(Position, barDimensions, YELLOW, (Color){10, 10, 10, 255}, true);
  EnemyBullet::Plr = this;
  addChild(dashBar);
}

Player::~Player() {
  EnemyBullet::Plr = nullptr;
}

void Player::Init() {};

void Player::setCam(CameraEntity* camra) {
  cam = camra;
}

float Player::getLifetime() {
  return lifetime;
}

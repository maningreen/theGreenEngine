#include "player.hpp"
#include "afterimage.hpp"
#include "bars.hpp"
#include "border.hpp"
#include "enemy.hpp"
#include "engine/entity.hpp"
#include "healthManager.hpp"
#include "include.h"
#include "nodeBullet.hpp"
#include "particle.hpp"
#include "raylib.h"
#include "raymath.h"
#include <cmath>
#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <string>

#define max(a, b) (a < b ? b : a)
#define min(a, b) (a < b ? a : b)

int Player::upKey = KEY_W;
int Player::downKey = KEY_S;
int Player::leftKey = KEY_A;
int Player::rightKey = KEY_D;

int Player::dashKey = KEY_SPACE;
float Player::dashTime = .4;
float Player::dashSpeed = 3000;

float Player::maxHealth = 10;

const float Player::defaultSpeed = 4000;
const float Player::defaultFriction = 58;

float Player::dashControl = 3;
float Player::dashCooldown = 1.5;
float Player::dashRegenDelay = .7;
unsigned Player::maxDashCount = 3;

float Player::particleSpawnTime = 1.0f / 30.0f;

const float distance = 50;

float Player::hitboxRadius = 25;

#define barDimensions (Vector2) {10, 100}

Vector2 Player::getInput() {
  return getInputVector(Player::upKey,
      Player::downKey,
      Player::leftKey,
      Player::rightKey);
}

void Player::Render() {
  // get the mouse position (in cartesian)
  // draw our triangle
  DrawTriangle(Position,
      Position +
          (Vector2) {cosf(rotation) * distance, sinf(rotation) * distance},
      Position + (Vector2) {cosf(rotation + 4 * PI / 3) * distance,
                     sinf(rotation + 4 * PI / 3) * distance},
      YELLOW);
  DrawTriangle(Position,
      Position + (Vector2) {cosf(rotation + 2 * PI / 3) * distance,
                     sinf(rotation + 2 * PI / 3) * distance},
      Position +
          (Vector2) {cosf(rotation) * distance, sinf(rotation) * distance},
      YELLOW);

  // we draw them darn sqrs
  const float height = 10;
  const Vector2 dems = {dashCooldownBar->Dimensions.x, height};
  // maxDashCount is the amount we draw
  float offsetY = dashCooldownBar->Dimensions.y / maxDashCount;

  for(int i = 0; i < maxDashCount; i++) {
    DrawRectangleV((Vector2) {dashCooldownBar->Position.x,
                       dashCooldownBar->Position.y + (i * offsetY)},
        dems,
        dashCooldownBar->EmptyCol);
  }

  DrawCircleV(Position, 5, WHITE);
}

void Player::Process(float delta) {
  lifetime += delta;
  Vector2 inputDirection = getInput();
  velocity = inputDirection * delta * speed + velocity;
  Position = Position + velocity * delta;
  velocity = velocity * delta * friction;

  manageDash(delta);

  Border::wrapEntity(this);

  if(dashManager.isDashing() || Vector2LengthSqr(inputDirection))
    if(fmodf(lifetime - particleSpawnTime, particleSpawnTime) <= 1.0 / 60.0f)
      getRoot()->addChild(new Particle(Position,
          Vector2Scale(velocity + Vector2Scale(inputDirection, speed * delta), -1)));

  if(dashManager.isDashing()) {
    if(fmodf(dashManager.getDeltaDash(), .1) < 1.0f / 120.0f)
      getRoot()->addChild(new Afterimage(Position, rotation));

  } else if(IsKeyPressed(dashKey) && dashManager.canDash()) {
    Vector2 dashDirection = Vector2Length(inputDirection) > 0
                        ? Vector2Scale(inputDirection, dashSpeed)
                        : Vector2Scale(Vector2Normalize(velocity), dashSpeed);
    dashManager.beginDash(dashDirection);
    dashManager.removeDashProgress();
  } 

  if(healthManager->isDead())
    killDefered();

  if(dashManager.canDash() && IsKeyPressed(KEY_ENTER)) {
    dashManager.removeDashProgress();
    addChild(new NodeBullet(Position, cam->getMousePosition(), rotation));
  }

  manageBars();
  manageRotation();
}

void Player::manageBars() {
  manageBar(dashCooldownBar, 1, dashManager.getDashProgress() / dashManager.maxDashCount, true);
}

void Player::manageBar(Bar* b, int index, float p, bool shouldRender) {
  // bool for verticle b->growVert
  float offsetX = !b->ShrinkY ? -b->Dimensions.x / 2.0f
                              : distance + b->Dimensions.x * index;
  float offsetY =
      b->ShrinkY ? -b->Dimensions.y / 2.0f : distance + b->Dimensions.y * index;
  Vector2 finalPosition = {Position.x + offsetX, Position.y + offsetY};
  b->Position = finalPosition;
  b->ShouldRender = shouldRender;
  b->Progress = p;
}

void Player::manageRotation() {
  Vector2 mousePos = cam->getMousePosition();
  rotation = atan2f((mousePos.y - Position.y),
      mousePos.x - Position.x); // then it's as simple as b - a
}

void Player::manageDash(float delta) {
  if(dashManager.isDashing())
    velocity = dashManager.manageDash(delta, getInputVector(upKey, downKey, leftKey, rightKey));
  else 
    dashManager.manageDash(delta, getInputVector(upKey, downKey, leftKey, rightKey));
}

Player::Player(const std::string& name, Vector2 position, CameraEntity* camera)
    : dashManager(maxDashCount, dashTime, dashRegenDelay, dashControl, dashSpeed, dashCooldown), Entity2D(name, position), cam(camera) {

  healthManager = new HealthManager(maxHealth,
      BarManager(&Position,
          distance,
          Bar(Position,
              (Vector2) {barDimensions.y, barDimensions.x},
              RED,
              DARKGRAY,
              false)));
  addChild(healthManager);

  velocity = (Vector2) {0, 0};
  speed = defaultSpeed;
  friction = defaultFriction;

  dashCooldownBar =
      new Bar(Position, barDimensions, YELLOW, (Color) {10, 10, 10, 255}, true);
  addChild(dashCooldownBar);

  cam = new CameraEntity("Camera", this);
  addChild(cam);
}

Player::~Player() { Enemy::setPlayer(nullptr); }

void Player::Init() { Enemy::setPlayer(); };

float Player::getLifetime() { return lifetime; }

HealthManager* Player::getHealthManager() { return healthManager; }

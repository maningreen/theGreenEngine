#include "player.hpp"

#include "afterimage.hpp"
#include "attackNode.hpp"
#include "bars.hpp"
#include "border.hpp"
#include "enemy.hpp"
#include "engine/core.h"
#include "engine/entity.hpp"
#include "healthManager.hpp"
#include "include.h"
#include "inputManager.hpp"
#include "mod.hpp"
#include "nodeBullet.hpp"
#include "particle.hpp"
#include "raylib.h"
#include "raymath.h"

#include <cmath>
#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <string>

Key Player::upKey = KEY_W;
Key Player::downKey = KEY_S;
Key Player::leftKey = KEY_A;
Key Player::rightKey = KEY_D;

Key Player::dashKey = KEY_SPACE;

Key Player::shootKey = KEY_ENTER;
Key Player::shootKeyMouse = MOUSE_LEFT_BUTTON;

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

#define barDimensions (Vector2){10, 100}

void Player::manageInput(float delta, Vector2 input) {
  velocity = input * delta * speed + velocity;
  manageDash(delta);

  if((dashManager.isDashing() || Vector2LengthSqr(input)) && fmodf(lifetime, particleSpawnTime) <= 1.0 / 60.0f)
    addChild(new Particle(Position,
      Vector2Scale(velocity + Vector2Scale(input, speed * delta), -1)));
}

void Player::beginDash(Vector2 input) {
  if(dashManager.canDash()) {
    Vector2 dashDirection =
      Vector2Length(input) > 0
        ? Vector2Scale(input, dashSpeed)
        : Vector2Scale(Vector2Normalize(velocity), dashSpeed);
    dashManager.beginDash(dashDirection);
    dashManager.removeDashProgress();
  }
}

void Player::Render() {
  // get the mouse position (in cartesian)
  // draw our triangle
  DrawTriangle(Position,
    Position + (Vector2){cosf(rotation) * distance, sinf(rotation) * distance},
    Position + (Vector2){cosf(rotation + 4 * PI / 3) * distance,
                 sinf(rotation + 4 * PI / 3) * distance},
    YELLOW);
  DrawTriangle(Position,
    Position + (Vector2){cosf(rotation + 2 * PI / 3) * distance,
                 sinf(rotation + 2 * PI / 3) * distance},
    Position + (Vector2){cosf(rotation) * distance, sinf(rotation) * distance},
    YELLOW);

  // we draw them darn sqrs
  const float height = 10;
  const Vector2 dems = {dashCooldownBar->Dimensions.x, height};
  // maxDashCount is the amount we draw
  float offsetY = dashCooldownBar->Dimensions.y / maxDashCount;

  for(int i = 0; i < maxDashCount; i++) {
    DrawRectangleV((Vector2){dashCooldownBar->Position.x,
                     dashCooldownBar->Position.y + (i * offsetY)},
      dems,
      dashCooldownBar->EmptyCol);
  }

  DrawCircleV(Position, 5, WHITE);
}

void Player::Process(float delta) {
  lifetime += delta;

  Position = Position + velocity * delta;
  velocity = velocity * delta * friction;

  Border::wrapEntity(this);

  if(dashManager.isDashing() && fmodf(dashManager.getDeltaDash(), .1) < 1.0f / 120.0f)
      getRoot()->addChild(new Afterimage(Position, rotation));

  if(healthManager->isDead())
    killDefered();

  manageBars();
  manageRotation();
}

void Player::manageBars() {
  manageBar(dashCooldownBar,
    1,
    dashManager.getDashProgress() / dashManager.maxDashCount,
    true);
}

void Player::manageBar(Bar* b, int index, float p, bool shouldRender) {
  // bool for verticle b->growVert
  float offsetX =
    !b->ShrinkY ? -b->Dimensions.x / 2.0f : distance + b->Dimensions.x * index;
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
    velocity = dashManager.manageDash(
      delta, getInputVector(upKey, downKey, leftKey, rightKey));
  else
    dashManager.manageDash(
      delta, getInputVector(upKey, downKey, leftKey, rightKey));
}

void Player::fireBullet() {
  int nodeBulletCount = Engine::getAllChildrenWithTag(this, NodeBullet::tag).size();
  int nodeCount = AttackNode::getNodes().size();
  if(dashManager.getAvailableDashes() > 0 && nodeCount + nodeBulletCount < 3) {
    dashManager.removeDashProgress();
    addChild(new NodeBullet(Position, cam->getMousePosition(), rotation));
  }
}

Player::Player(const std::string& name, Vector2 position, CameraEntity* camera)
  : dashManager(maxDashCount,
      dashTime,
      dashRegenDelay,
      dashControl,
      dashSpeed,
      dashCooldown),
    Entity2D(name, position), cam(camera) {

  healthManager = new HealthManager(maxHealth,
    BarManager(&Position,
      distance,
      Bar(Position,
        (Vector2){barDimensions.y, barDimensions.x},
        RED,
        DARKGRAY,
        false)));
  addChild(healthManager);

  inputManager = new InputManager(upKey, downKey, leftKey, rightKey, keybinds());
  inputManager->addBind(keybind(
    shootKey,
    [this](){ fireBullet(); }
  ));
  inputManager->addBind(keybind(
    shootKeyMouse,
    true,
    [this](){ fireBullet(); }
  ));
  inputManager->addBind(keybindAlt(
    dashKey,
    [this](Vector2 i){ beginDash(i); }
  ));
  inputManager->addVectorBind([this](float delta, Vector2 i) {
      this->manageInput(delta, i);
  });
  addChild(inputManager);

  velocity = (Vector2){0, 0};
  speed = defaultSpeed;
  friction = defaultFriction;

  dashCooldownBar =
    new Bar(Position, barDimensions, YELLOW, (Color){10, 10, 10, 255}, true);
  addChild(dashCooldownBar);

  cam = new CameraEntity("Camera", this);
  addChild(cam);

  modManager = new ModManager;
}

Player::~Player() { 
  Enemy::setPlayer(nullptr); 
  delete healthManager;
  delete dashCooldownBar;
  delete inputManager;
  delete cam;
  delete modManager;
}

void Player::Init() { Enemy::setPlayer(); };

float Player::getLifetime() { return lifetime; }

HealthManager* Player::getHealthManager() { return healthManager; }

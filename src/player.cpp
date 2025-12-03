#include "player.hpp"

#include "afterimage.hpp"
#include "attackNode.hpp"
#include "bars.hpp"
#include "border.hpp"
#include "dashManager.hpp"
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
#include <iostream>
#include <string>

Key Player::upKey = KEY_W;
Key Player::downKey = KEY_S;
Key Player::leftKey = KEY_A;
Key Player::rightKey = KEY_D;

Key Player::dashKey = KEY_SPACE;

Key Player::shootKey = KEY_ENTER;
Key Player::shootKeyMouse = MOUSE_LEFT_BUTTON;

Player* Player::player = nullptr;

float Player::maxHealth = 10;

const float Player::defaultSpeed = 4000;

const float Player::defaultDashTime = .4;
const float Player::defaultDashSpeed = 3000;
const float Player::defaultDashControl = 3;
const float Player::defaultDashCooldown = 1.5;
const float Player::defaultDashRegenDelay = .7;
const unsigned Player::defaultMaxDashCount = 3;

float Player::particleSpawnTime = 1.0f / 30.0f;

const float distance = 50;

float Player::hitboxRadius = 25;

#define barDimensions (Vector2){10, 100}

void Player::manageInput(float delta, Vector2 input) {
  velocity = input * delta * speed + velocity;
  manageDash(delta);

  if((dashManager.isDashing() || Vector2LengthSqr(input)) && fmodf(lifetime, particleSpawnTime) <= 1.0 / 60.0f)
    addChild(new Particle(position,
      Vector2Scale(velocity + Vector2Scale(input, speed * delta), -1)));
}

void Player::beginDash(Vector2 input) {
  if(dashManager.canDash()) {
    Vector2 dashDirection =
      Vector2Length(input) > 0
        ? input
        : Vector2Normalize(velocity);
    dashManager.beginDash(dashDirection);
    dashManager.removeDashProgress();
    modManager->onDash(this);
  }
}

void Player::render() {
  // get the mouse position (in cartesian)
  // draw our triangle
  DrawTriangle(position,
    position + (Vector2){cosf(rotation) * distance, 
                         sinf(rotation) * distance},
    position + (Vector2){cosf(rotation + 4 * PI / 3) * distance,
                         sinf(rotation + 4 * PI / 3) * distance},
    YELLOW);
  DrawTriangle(position,
    position + (Vector2){cosf(rotation + 2 * PI / 3) * distance,
                 sinf(rotation + 2 * PI / 3) * distance},
    position + (Vector2){cosf(rotation) * distance, sinf(rotation) * distance},
    YELLOW);

  // we draw them darn sqrs
  const float height = 10;
  const Vector2 dems = {dashCooldownBar->Dimensions.x, height};
  // maxDashCount is the amount we draw
  float offsetY = dashCooldownBar->Dimensions.y / dashManager.maxDashCount;

  for(int i = 0; i < dashManager.maxDashCount; i++) {
    DrawRectangleV((Vector2){dashCooldownBar->position.x,
                     dashCooldownBar->position.y + (i * offsetY)},
      dems,
      dashCooldownBar->EmptyCol);
  }

  DrawCircleV(position, 5, WHITE);
}

void Player::process(float delta) {
  lifetime += delta;

  position = position + velocity * delta;
  velocity = velocity * Entity2D::friction;

  Border::wrapEntity(this);

  if(dashManager.isDashing() && fmodf(dashManager.getDeltaDash(), .1) < 1.0f / 120.0f)
      getRoot()->addChild(new Afterimage(position, rotation));

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
  Vector2 finalPosition = {position.x + offsetX, position.y + offsetY};
  b->position = finalPosition;
  b->ShouldRender = shouldRender;
  b->Progress = p;
}

void Player::manageRotation() {
  Vector2 mousePos = cam->getMousePosition();
  rotation = atan2f((mousePos.y - position.y),
    mousePos.x - position.x); // then it's as simple as b - a
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
    addChild(new NodeBullet(position, cam->getMousePosition(), rotation));
  }
}

Player::Player(const std::string& name, Vector2 position)
  : dashManager(defaultMaxDashCount,
      defaultDashTime,
      defaultDashRegenDelay,
      defaultDashControl,
      defaultDashSpeed),
    Entity2D(name, position) {

  Player::player = this;

  healthManager = new HealthManager(maxHealth,
    BarManager(&position,
      distance,
      Bar(position,
        (Vector2){barDimensions.y, barDimensions.x},
        RED,
        DARKGRAY,
        false)));
  addChild(healthManager);

  inputManager = new InputManager(upKey, downKey, leftKey, rightKey, keybinds());
  inputManager->addBind(keybind(
    shootKey,
    [this](){ 
      this->fireBullet(); 
    }
  ));
  inputManager->addBind(keybind(
    shootKeyMouse,
    true,
    [this](){ 
      this->fireBullet(); 
    }
  ));
  inputManager->addBind(keybindAlt(
    dashKey,
    [this](Vector2 i){ 
      this->beginDash(i); 
    }
  ));
  inputManager->addVectorBind([this](float delta, Vector2 i) {
      this->manageInput(delta, i);
  });

  Enemy::addSpawnHook([this](Enemy* x){
    this->modManager->onEnemySpawn(this, x);
  });

  Enemy::addDeathHook([this](Enemy* x){
    this->modManager->onEnemyKill(this, x);
  });

  addChild(inputManager);

  velocity = (Vector2){0, 0};
  speed = defaultSpeed;

  dashCooldownBar =
    new Bar(position, barDimensions, YELLOW, (Color){10, 10, 10, 255}, true);
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

void Player::init() { 
  Enemy::setPlayer(); 
  modManager->loadMods(this);
};

float Player::getLifetime() { return lifetime; }

HealthManager* Player::getHealthManager() { return healthManager; }
DashManager* Player::getDashManager() { return &dashManager; }
InputManager* Player::getInputManager() { return inputManager; }

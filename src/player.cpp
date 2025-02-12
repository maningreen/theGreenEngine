#include "player.hpp"
#include "camera.hpp"
#include "entity.hpp"
#include "core.h"
#include "border.hpp"
#include "bullet.hpp"
#include "afterimage.hpp"
#include "bars.hpp"
#include <cmath>
#include <cstdlib>
#include <raylib.h>
#include <raymath.h>
#include <string>
#include <iostream>

#define max(a, b) (a < b ? b : a)
#define min(a, b) (a < b ? a : b)

const int Player::upKey = KEY_W;
const int Player::downKey = KEY_S;
const int Player::leftKey = KEY_A;
const int Player::rightKey = KEY_D;

const int Player::shootKey = 0;
const int Player::shootKeyMouse = MOUSE_BUTTON_LEFT;

const int Player::dashKey = KEY_SPACE;
float Player::dashTime = .4;
float Player::dashSpeed = 2500;
float Player::dashControl = .5;

const float Player::defaultSpeed = 4000;
const float Player::defaultFriction = 58;

float Player::dashCooldown = .7;

const float distance = 50;

#define barDimensions (Vector2){100, 10}

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
  Vector2 inputDirection = getInput();
  Velocity = Vector2Add(Vector2Scale(inputDirection, delta * Speed), Velocity);
  Position = Vector2Add(Position, Vector2Scale(Velocity, delta));
  Velocity = Vector2Scale(Velocity, delta * Friction);
  wrapPosition();
  timeSinceDash += delta;
  if(dashing) {
    timeDashing += delta;
    Velocity = dashDirection;
    //i wanna do something funny.
    //that funny is changing the dashDirection.
    //based on the playerInput.
    if(Vector2LengthSqr(inputDirection) != 0) {
      dashDirection = Vector2Scale(Vector2Normalize(Vector2Add(dashDirection, Vector2Scale(inputDirection, (delta / dashTime) * dashSpeed / dashControl))), dashSpeed);
    }
    if(fmodf(timeDashing, .1) < 1.0f / 60.0f)
      addChild(new Afterimage(Position, Rotation));
    if(timeDashing > dashTime) {
      dashing = false;
      timeDashing = 0;
    }
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
  addChild(new Bullet(Vector2Add(Position, (Vector2){cosf(Rotation) * offsetAhead, -sinf(Rotation) * offsetAhead}), Rotation));
}

//returns true if out of bounds
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
  Vector2 offset = Vector2Add(Position, (Vector2){-barDimensions.x / 2.0f, distance + barDimensions.y});
  dashCooldownBar->Position = offset;
  dashCooldownBar->TargetProgress = min(timeSinceDash / dashCooldown, 1);
  dashCooldownBar->ShouldRender = dashCooldownBar->TargetProgress != 1;
  offset = Vector2Add(offset, (Vector2){0, barDimensions.y * 2});
  dashBar->Position = offset;
  dashBar->ShouldRender = dashing;
  dashBar->TargetProgress = timeDashing / dashTime;
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
  timeDashing = 0;
  dashDirection = Vector2Zero();
  dashCooldownBar = new Bar(Position, barDimensions, YELLOW, (Color){10, 10, 10, 255}, false);
  addChild(dashCooldownBar);
  timeSinceDash = 0;
  dashBar = new Bar(Position, barDimensions, YELLOW, (Color){10, 10, 10, 255}, false);
  addChild(dashBar);
}

void Player::setCam(CameraEntity* camra) {
  cam = camra;
}

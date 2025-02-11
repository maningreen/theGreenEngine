#include "player.hpp"
#include "entity.hpp"
#include "core.h"
#include "border.hpp"
#include "bullet.hpp"
#include <cmath>
#include <cstdlib>
#include <raylib.h>
#include <raymath.h>
#include <string>

const int Player::upKey = KEY_W;
const int Player::downKey = KEY_S;
const int Player::leftKey = KEY_A;
const int Player::rightKey = KEY_D;

const int Player::shootKey = KEY_SPACE;
const int Player::shootKeyMouse = MOUSE_BUTTON_LEFT;

const int Player::dashKey = KEY_E;
float Player::dashTime = .4;
float Player::dashSpeed = 2500;

const float Player::defaultSpeed = 4000;
const float Player::defaultFriction = 58;

Vector2 Player::getInput() {
  return !dashing ? Player::getInput(Player::upKey, Player::downKey, Player::leftKey, Player::rightKey) : (Vector2){0, 0};
}

Vector2 Player::getInput(int u, int d, int l, int r) {
  Vector2 out = (Vector2){(float)IsKeyDown(r) - (float)IsKeyDown(l), (float)IsKeyDown(d) - (float)IsKeyDown(u)};
  return out;
}

void Player::Render() {
  //get the mouse position (in cartesian)
  Vector2 mousePos = Vector2Subtract(GetMousePosition(), (Vector2){GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f});
  float mouseAngle = atan2f(-mousePos.y, mousePos.x);
  //draw our triangle
  const float distance = 50;
  DrawTriangle(
      Position,
      Vector2Add(Position, (Vector2){cosf(mouseAngle + 4 * M_PI / 3) * distance, -sinf(mouseAngle + 4 * M_PI / 3) * distance}),
      Vector2Add(Position, (Vector2){cosf(mouseAngle) * distance, -sinf(mouseAngle) * distance}),
        YELLOW
      );
  DrawTriangle(
      Position,
      Vector2Add(Position, (Vector2){cosf(mouseAngle) * distance, -sinf(mouseAngle) * distance}),
      Vector2Add(Position, (Vector2){cosf(mouseAngle + 2 * M_PI / 3) * distance, -sinf(mouseAngle + 2 * M_PI / 3) * distance}),
        YELLOW
      );

  DrawCircleV(Position, 5, WHITE);
}

void Player::Process(float delta) {
  Vector2 inputDirection = Vector2Scale(getInput(), Speed * delta);
  Velocity = Vector2Add(inputDirection, Velocity);
  Position = Vector2Add(Position, Vector2Scale(Velocity, delta));
  Velocity = Vector2Scale(Velocity, delta * Friction);
  printf("%f\n", Vector2Length(Velocity));
  wrapPosition();
  if(dashing) {
    timeDashing += delta;
    Velocity = dashDirection;
    if(timeDashing > dashTime) {
      dashing = false;
      timeDashing = 0;
    }
  }
  if(IsKeyPressed(dashKey) && !dashing) {
    dashing = true;
    dashDirection = Vector2LengthSqr(inputDirection) != 0 ? Vector2Scale(inputDirection, dashSpeed / (Speed * delta)) : Vector2Scale(Vector2Normalize(Velocity), dashSpeed);
  }
  if(IsKeyPressed(shootKey) || IsMouseButtonPressed(shootKeyMouse))
    SpawnBullet();
}

void Player::SpawnBullet() {
  const float offsetAhead = 30;
  Vector2 mousePos = Vector2Subtract(GetMousePosition(), (Vector2){GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f});
  float mouseAngle = atan2f(-mousePos.y, mousePos.x);
  addChild(new Bullet(Vector2Add(Position, (Vector2){cosf(mouseAngle) * offsetAhead, -sinf(mouseAngle) * offsetAhead}), mouseAngle));
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

Player::Player(const std::string& name, Vector2 position) : Entity2D(name, position) {
  Velocity = (Vector2){0,0};
  Speed = defaultSpeed;
  Friction = defaultFriction;
  dashing = false;
  timeDashing = 0;
  dashDirection = Vector2Zero();
}

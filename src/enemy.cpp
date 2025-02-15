#include "enemy.hpp"
#include "border.hpp"
#include <raylib.h>
#include <raymath.h>

float Enemy::Radius = 30;
float Enemy::Speed = 400;
Color Enemy::Colour = RED;

Enemy::Enemy(Player* pl, Vector2 pos) : Entity2D("Enemy", pos), plr(pl) {
  Velocity = {0, 0};
  TargetPos = {0, 0};
  addTag("Enemy");
}

Enemy::~Enemy() {}

void Enemy::Process(float delta) {
  WrapPosition();
}

void Enemy::Render() {
  //hmm what do we want for our enemies???
  //hear me out:
  //circle
  DrawCircleV(Position, Radius, Colour);
}

Vector2 Enemy::GetNextTargetPosition() {
  return Vector2Zero();
}

void Enemy::WrapPosition() {
  if(Position.x < -Border::Length)
    Position.x += Border::Length * 2;
  else if(Position.x > Border::Length)
    Position.x -= Border::Length * 2;
  if(Position.y < -Border::Length)
    Position.y += Border::Length * 2;
  else if(Position.y > Border::Length)
    Position.y -= Border::Length * 2;
}

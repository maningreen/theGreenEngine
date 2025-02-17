#include "enemy.hpp"
#include "border.hpp"
#include "entity.hpp"
#include "core.h"
#include "bullet.hpp"
#include "bars.hpp"
#include <raylib.h>
#include <raymath.h>
#include <vector>

float Enemy::Radius = 30;
float Enemy::Speed = 4000;
Color Enemy::Colour = RED;

float Enemy::MaxHealth = 10;

#define barDimensions (Vector2){Enemy::Radius, 10}

Enemy::Enemy(Player* pl, Vector2 pos) : Entity2D("Enemy", pos), plr(pl) {
  Velocity = {0, 0};
  TargetPos = {0, 0};
  addChild(new Bar(Vector2Zero(), barDimensions, RED, GRAY, false));
  addTag("Enemy");
  Bullet::Enemies.push_back(this);
}

Enemy::~Enemy() {
  Bullet::Enemies.clear();
}

void Enemy::Init() {
  plr = (Player*)Engine::getFirstEntityIndexWithName(getRoot()->Children, "Player");
}

void Enemy::Process(float delta) {
  Velocity = getNextVelocity(delta);
  Position = Vector2Add(Position, Vector2Scale(Velocity, delta));
  WrapPosition();
}

Vector2 Enemy::getNextVelocity(float delta) {
  return Vector2Zero();
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

Vector2 Enemy::getShortestVectorToPlayer() {
  Vector2 vectorToPlayer = Vector2Subtract(plr->Position, Position);
  if(vectorToPlayer.x >= Border::Length)
    vectorToPlayer.x -= Border::Length * 2.0f;
  else if(vectorToPlayer.x <= -Border::Length)
    vectorToPlayer.x += Border::Length * 2.0f;
  if(vectorToPlayer.y >= Border::Length)
    vectorToPlayer.y -= Border::Length * 2.0f;
  else if(vectorToPlayer.y <= -Border::Length)
    vectorToPlayer.y += Border::Length * 2.0f;
  return vectorToPlayer;
}

float Enemy::getHealth() {
  return health;
}

void Enemy::applyDamage(float damage) {
  health -= damage;
}

bool Enemy::isAlive() {
  return health > 0;
}

void Enemy::manageBar() {
}

#include "enemy.hpp"
#include "border.hpp"
#include "entity.hpp"
#include "core.h"
#include <raylib.h>
#include <raymath.h>

float Enemy::Radius = 30;
float Enemy::Speed = 4000;
Color Enemy::Colour = RED;

Enemy::Enemy(Player* pl, Vector2 pos) : Entity2D("Enemy", pos), plr(pl) {
  Velocity = {0, 0};
  TargetPos = {0, 0};
  addTag("Enemy");
}

Enemy::~Enemy() {}

void Enemy::Init() {
  plr = (Player*)Engine::getFirstEntityIndexWithName(getRoot()->Children, "Player");
}

void Enemy::Process(float delta) {
  Vector2 vectorToPlayer = getShortestVectorToPlayer();
  float dist = Vector2Length(vectorToPlayer);
  Vector2 addedVel = Vector2Scale(vectorToPlayer, Speed <= dist ? Speed / dist : 1);
  Velocity = Vector2Add(Velocity, Vector2Scale(addedVel, delta));
  Position = Vector2Add(Position, Vector2Scale(Velocity, delta));
  WrapPosition();
}

void Enemy::Render() {
  //hmm what do we want for our enemies???
  //hear me out:
  //circle
  DrawCircleV(Position, Radius, Colour);
  DrawLineEx(Position, Vector2Add(Position, getShortestVectorToPlayer()), 10, WHITE);
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

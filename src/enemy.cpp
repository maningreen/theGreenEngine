#include "enemy.hpp"
#include "border.hpp"
#include "engine/entity.hpp"
#include "engine/core.h"
#include "playerBullet.hpp"
#include "bars.hpp"
#include "include.h"
#include <vector>
#include <iostream>

float Enemy::DefaultRadius= 30;
float Enemy::Speed = 4000;
Color Enemy::Colour = RED;

#define barDimensions (Vector2){Radius * 2, 10}

Enemy::Enemy(Vector2 pos) : Entity2D("Enemy", pos) {
  Velocity = {0, 0};
  TargetPos = {0, 0};
  Radius = DefaultRadius;
  healthManager = new HealthManager(10,BarManager(&Position, Radius * 1.5f, new Bar(Vector2Zero(), barDimensions, RED, DARKGRAY, false)));
  addChild(healthManager);
  addTag("Enemy");
  PlayerBullet::enemies.push_back(this);
}

Enemy::~Enemy() {
  for(int i = 0; i < PlayerBullet::enemies.size(); i++)
    if(PlayerBullet::enemies[i] == this) //the most beautiful nesting you'll ever see
      PlayerBullet::enemies.erase(PlayerBullet::enemies.begin() + i);
}

void Enemy::Init() {
  setPlayer();
}

void Enemy::Process(float delta) {
  Velocity = getNextVelocity(delta);
  Position = Vector2Add(Position, Vector2Scale(Velocity, delta));
  manageHealthBar(Radius);
  WrapPosition();
}

Vector2 Enemy::getNextVelocity(float delta) {
  return Vector2Zero();
}

void Enemy::Render() {
  //hmm what do we want for our enemies???
  //hear me out:
  //circle
  DrawCircleV(Position, Radius, Colour); //WHOOOOOO
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

void Enemy::manageHealthBar(float r) {
  healthManager->targetDistance = r * 1.5f;
  if(healthManager->isDead())
    valid = false;
}

void Enemy::setPlayer() {
  plr = (Player*)Engine::getFirstEntityIndexWithName(getRoot()->Children, "Player");
}

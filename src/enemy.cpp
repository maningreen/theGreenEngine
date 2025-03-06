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
float Enemy::friction = 58;
Color Enemy::Colour = RED;
Player* Enemy::plr = nullptr;

#define barDimensions (Vector2){Radius * 2, 10}

Enemy::Enemy(Vector2 pos) : Entity2D("Enemy", pos) {
  Velocity = {0, 0};
  TargetPos = {0, 0};
  Radius = DefaultRadius;
  healthManager = new HealthManager(10,BarManager(&Position, Radius * 1.5f, Bar(Vector2Zero(), barDimensions, RED, DARKGRAY, false)));
  healthManager->getBar()->ShouldRender = true;
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
  manageHealthBar(Radius);
  WrapPosition();
}

void Enemy::Render() {
  //hmm what do we want for our enemies???
  //hear me out:
  //circle
  DrawCircleV(Position, Radius, Colour); //WHOOOOOO
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
  return Border::getShortestPathToPoint(this, plr->Position);
}

void Enemy::manageHealthBar(float r) {
  healthManager->targetDistance = r * 1.5f;
  if(healthManager->isDead())
    valid = false;
}

void Enemy::setPlayer() {
  plr = (Player*)Engine::searchTreeForEntity(&getRoot()->Children, "Player");
}

Vector2 Enemy::getClosestPointToPlayerWithDistance(float dist) {
  // normalize the offset
  if(plr == nullptr)
    return Vector2Zero();
  Vector2 diff = getShortestVectorToPlayer();
  Vector2 normalizedOffset = Vector2Scale(Vector2Normalize(diff), dist);
  return Border::wrapPos(Vector2Add(normalizedOffset, Position));
}

#include "enemy.hpp"
#include "border.hpp"
#include "entity.hpp"
#include "core.h"
#include "playerBullet.hpp"
#include "bars.hpp"
#include "include.h"
#include <vector>
#include <iostream>

float Enemy::DefaultRadius= 30;
float Enemy::Speed = 4000;
Color Enemy::Colour = RED;

float Enemy::MaxHealth = 10;

#define barDimensions (Vector2){Radius * 2, 10}

Enemy::Enemy(Vector2 pos) : Entity2D("Enemy", pos) {
  Velocity = {0, 0};
  TargetPos = {0, 0};
  Radius = DefaultRadius;
  healthBar = new Bar(Vector2Zero(), barDimensions, RED, DARKGRAY, false);
  addChild(healthBar);
  addTag("Enemy");
  PlayerBullet::enemies.push_back(this);
  health = MaxHealth;
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
  manageBar();
  manageHeath();
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

void Enemy::applyDamage(float damage) {
  health -= damage;
}

bool Enemy::isAlive() {
  return health > 0;
}

void Enemy::manageBar() {
  healthBar->ShouldRender = true;
  healthBar->TargetProgress = health / MaxHealth;
  healthBar->Position = Vector2Add(Position, (Vector2){-barDimensions.x / 2.0f, Radius * 1.5f});
  *(float*)(&healthBar->Dimensions + (healthBar->ShrinkY ? sizeof(float) : 0)) = Radius * 2; //i'm not fucking sorry for this
}                                                                                            //it's beautiful and i love it
                                                                                             //it just if it grows y it sets the y
                                                                                             //otherwise it'll set the x
void Enemy::manageHeath() {
  if(health <= 0)
    valid = false;
}

void Enemy::setPlayer() {
  plr = (Player*)Engine::getFirstEntityIndexWithName(getRoot()->Children, "Player");
}

#include "enemy.hpp"
#include "border.hpp"
#include "engine/entity.hpp"
#include "engine/core.h"
#include "healthManager.hpp"
#include "enemyBullet.hpp"
#include "playerBullet.hpp"
#include "bars.hpp"
#include "include.h"
#include <vector>
#include <cmath>

float Enemy::DefaultRadius= 30;
float Enemy::Speed = 4000;
float Enemy::friction = 58;
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
  Position = Vector2Add(Position, Vector2Scale(Velocity, delta));
  Velocity = Vector2Scale(Velocity, friction * delta);
  stateTime += delta;
  if(plr != nullptr)
    manageStates(delta);
  Border::wrapEntity(this);
}

void Enemy::Render() {
  //hmm what do we want for our enemies???
  //hear me out:
  //circle
  DrawCircleV(Position, Radius, Colour); //WHOOOOOO
}

Vector2 Enemy::getShortestVectorToPlayer() const {
  if(plr == nullptr)
    return Vector2Zero();
  return Border::getShortestPathToPoint(Position, plr->Position);
}

void Enemy::manageHealthBar(float r) {
  healthManager->targetDistance = r * 1.5f;
  if(healthManager->isDead())
    killDefered();
}

void Enemy::setPlayer() {
  plr = (Player*)Engine::searchTreeForEntity(&getRoot()->Children, "Player");
}

void Enemy::setPlayer(Player* p) {
  plr = p;
}

Vector2 Enemy::getClosestPointToPlayerWithDistance(float dist) const {
  // normalize the offset
  if(plr == nullptr)
    return Vector2Zero();
  Vector2 shortestPathToPlayer = getShortestVectorToPlayer();
  //lets get a model
  //--------->
  //say that is our vector to our player so our player is 
  //--------->* here
  //i want a distance around there so we would have a vector of
  //<---
  //ish
  //so that's what we wanna do
  Vector2 vectorFromPlayer = Vector2Scale(Vector2Normalize(shortestPathToPlayer), -dist);
  Vector2 globalPos = Vector2Add(vectorFromPlayer, plr->Position);
  Vector2 vectorToGlobal = Border::wrapPos(globalPos);
  //then we do some shmath
  return vectorToGlobal;
}

unsigned char Enemy::getState() const {
  return state;
}

void Enemy::setState(unsigned char s) {
  state = s;
  resetStateTime();
}

float Enemy::getAngleToPlayer() const {
  Vector2 vToP = getShortestVectorToPlayer();
  return atan2f(vToP.y, vToP.x);
}

void ManageStates() {
}

float Enemy::getStateTime() const {
  return stateTime;
}

Player* Enemy::getPlayer() {
  return plr;
}

HealthManager* Enemy::getHealthManager() const {
  return healthManager;
}

void Enemy::resetStateTime() {
  stateTime = 0;
}

EnemyBullet* Enemy::fireBullet(float angle, float lifetime, Color col) const {
  EnemyBullet* bul = new EnemyBullet(Position, angle, col, true, lifetime);
  getRoot()->addChild(bul);
  return bul;
}

EnemyBullet* Enemy::fireBullet(float angle, float lifetime, float s, Color col) const {
  EnemyBullet* bul = new EnemyBullet(Position, angle, col, true, lifetime, s);
  getRoot()->addChild(bul);
  return bul;
}

EnemyBullet* Enemy::fireBullet(float angle, float lifetime, float s, float dmg, Color col) const {
  EnemyBullet* bul = new EnemyBullet(Position, angle, col, true, lifetime, s, dmg);
  getRoot()->addChild(bul);
  return bul;
}

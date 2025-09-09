#include "enemy.hpp"
#include "border.hpp"
#include "engine/entity.hpp"
#include "engine/core.h"
#include "healthManager.hpp"
#include "enemyBullet.hpp"
#include "healthPack.hpp"
#include "bars.hpp"
#include "include.h"
#include <raylib.h>
#include <raymath.h>
#include <vector>
#include <cmath>

float Enemy::DefaultRadius= 30;
float Enemy::Speed = 4000;
float Enemy::friction = 58;
Entity2D* Enemy::plr = nullptr;

float Enemy::droppedHealthHP = 1;

#define barDimensions (Vector2){Radius * 2, 10}

Enemy::Enemy(Vector2 pos) : Entity2D("Enemy", pos) {
  Velocity = {0, 0};
  TargetPos = {0, 0};
  Radius = DefaultRadius;
  healthManager = new HealthManager(10,BarManager(&Position, Radius * 1.5f, Bar(Vector2Zero(), barDimensions, RED, DARKGRAY, false)));
  healthManager->getBar()->ShouldRender = true;
  addChild(healthManager);
  addTag("Enemy");
}

Enemy::~Enemy() {
  Player::removeEnemy(this);
}

void Enemy::Death() {
  dropHealth();
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
  //circle :3
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

void Enemy::setPlayer(Entity2D* p) {
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

Entity2D* Enemy::getPlayer() {
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

Vector2 getHealthPackVel(Vector2 enVel) {
  float r = 2 * rand() / (float)RAND_MAX - 1; // range = (-1, 1)
  float weight = r * r * r * r; // r^4
  float aOfEn = atan2f(enVel.x, -enVel.y); // find the angle of the enemies velocity
  float nT = weight * M_PI + aOfEn; // add the random weighted angle to the actual angle of the velocity
  return (Vector2){ cos(nT), sin(nT) }; // un-angle it w/ trig
}

// if not provided uses droppedHealthHP
// otherwise, same as the more verbose overload
void Enemy::dropHealthPack() {
  // :p
  Vector2 v = getHealthPackVel(Velocity);
  Entity* root = getRoot();
  HealthPack* h = new HealthPack(Position, Velocity, droppedHealthHP);
  root->addChild(h);
}

void Enemy::dropHealthPack(float hp, Entity* root) {
  Vector2 v = getHealthPackVel(Velocity);
  HealthPack* h = new HealthPack(Position, Velocity, hp);
  root->addChild(h);
}

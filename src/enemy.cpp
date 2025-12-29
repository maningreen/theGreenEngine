#include "enemy.hpp"
#include "bars.hpp"
#include "border.hpp"
#include "enemyBullet.hpp"
#include "engine/core.h"
#include "engine/entity.hpp"
#include "healthManager.hpp"
#include "healthPack.hpp"
#include "include.h"
#include <cmath>

float Enemy::DefaultRadius = 30;
float Enemy::Speed = 4000;
Entity2D* Enemy::plr = nullptr;

float Enemy::droppedHealthHP = 1;

const std::string Enemy::tag = "Enemy";

std::vector<std::function<void(Enemy*)>> Enemy::onSpawnHooks;
std::vector<std::function<void(Enemy*)>> Enemy::onDeathHooks;

#define barDimensions (Vector2) {radius * 2, 10}

Enemy::Enemy(Vector2 pos) : Entity2D("Enemy", pos) {
  velocity = {0, 0};
  targetPos = {0, 0};
  radius = DefaultRadius;
  healthManager = new HealthManager(10,
    BarManager(&position,
      radius * 1.5f,
      Bar(
        Vector2Zero(), 
        barDimensions,
        RED, 
        DARKGRAY, 
        false
      )
    )
  );
  healthManager->getBar()->ShouldRender = true;
  colour = PINK;
  addChild(healthManager);
  addTag(tag);
}

Enemy::~Enemy() {
}

void Enemy::death() { 
  for(std::function<void(Enemy*)> f : onDeathHooks)
    f(this);
  if(healthManager->isDead())
    dropHealth(); 
  else 
    valid = true;
}

void Enemy::init() { 
  setPlayer(); 
  for(
    std::function<void(Enemy *)>* f = onSpawnHooks.data(); 
    f < onSpawnHooks.data() + onSpawnHooks.size();
    (*(f++))(this)
  );
  onSpawn();
}

void Enemy::process(float delta) {
  manageHealthBar(radius);
  position = position + velocity * delta;
  velocity = velocity * friction;
  stateTime += delta;
  if(plr != nullptr)
    manageStates(delta);
  Border::wrapEntity(this);
}

void Enemy::render() {
  // hmm what do we want for our enemies???
  // hear me out:
  // circle :3
  DrawCircleV(position, radius, colour); // WHOOOOOO
}

Vector2 Enemy::getShortestVectorToPlayer() const {
  if(plr == nullptr)
    return Vector2Zero();
  return Border::getShortestPathToPoint(position, plr->position);
}

void Enemy::manageHealthBar(float r) {
  healthManager->targetDistance = r * 1.5f;
  if(healthManager->isDead())
    killDefered();
}

void Enemy::setPlayer() {
  plr = (Player*)Engine::searchTreeForEntity(&getRoot()->children, "Player");
}

void Enemy::setPlayer(Entity2D* p) { 
  plr = p; 
}

void Enemy::addSpawnHook(std::function<void (Enemy *)> x) {
  onSpawnHooks.push_back(x);
}

void Enemy::addDeathHook(std::function<void (Enemy *)> x) {
  onDeathHooks.push_back(x);
}

Vector2 Enemy::getClosestPointToPlayerWithDistance(float dist) const {
  // normalize the offset
  if(plr == nullptr)
    return Vector2Zero();
  Vector2 shortestPathToPlayer = getShortestVectorToPlayer();
  // lets get a model
  //--------->
  // say that is our vector to our player so our player is
  //--------->* here
  // i want a distance around there so we would have a vector of
  //<---
  // ish
  // so that's what we wanna do
  Vector2 vectorFromPlayer =
      Vector2Scale(Vector2Normalize(shortestPathToPlayer), -dist);
  Vector2 globalPos = Vector2Add(vectorFromPlayer, plr->position);
  Vector2 vectorToGlobal = Border::wrapPos(globalPos);
  // then we do some shmath
  return vectorToGlobal;
}

unsigned char Enemy::getState() const { return state; }

void Enemy::setState(unsigned char s) {
  state = s;
  resetStateTime();
}

float Enemy::getAngleToPlayer() const {
  Vector2 vToP = getShortestVectorToPlayer();
  return atan2f(vToP.y, vToP.x);
}

void ManageStates() {}

float Enemy::getStateTime() const { return stateTime; }

Entity2D* Enemy::getPlayer() { return plr; }

HealthManager* Enemy::getHealthManager() { return healthManager; }

void Enemy::resetStateTime() { stateTime = 0; }

EnemyBullet* Enemy::fireBullet(float angle, float lifetime, Color col) const {
  EnemyBullet* bul = new EnemyBullet(position, angle, col, true, lifetime);
  getRoot()->addChild(bul);
  return bul;
}

EnemyBullet* Enemy::fireBullet(float angle, float lifetime, float s,
    Color col) const {
  EnemyBullet* bul = new EnemyBullet(position, angle, col, true, lifetime, s);
  getRoot()->addChild(bul);
  return bul;
}

EnemyBullet* Enemy::fireBullet(float angle, float lifetime, float s, float dmg,
    Color col) const {
  EnemyBullet* bul =
      new EnemyBullet(position, angle, col, true, lifetime, s, dmg);
  getRoot()->addChild(bul);
  return bul;
}

Vector2 getHealthPackVel(Vector2 enVel) {
  float r = 2 * rand() / (float)RAND_MAX - 1; // range = (-1, 1)
  float weight = r * r * r * r;               // r^4
  float aOfEn =
      atan2f(enVel.x, -enVel.y);       // find the angle of the enemies velocity
  float nT = weight * M_PI + aOfEn;    // add the random weighted angle to the
                                       // actual angle of the velocity
  return (Vector2) {cos(nT), sin(nT)}; // un-angle it w/ trig
}

// if not provided uses droppedHealthHP
// otherwise, same as the more verbose overload
void Enemy::dropHealthPack() {
  Vector2 v = getHealthPackVel(velocity);
  Entity* root = getRoot();
  HealthPack* h = new HealthPack(position, velocity, droppedHealthHP);
  getParent()->addChild(h);
}

void Enemy::dropHealthPack(float hp) {
  Vector2 v = getHealthPackVel(velocity);
  HealthPack* h = new HealthPack(position, velocity, hp);
  getParent()->addChild(h);
}

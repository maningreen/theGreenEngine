#include "enemy.hpp"

#include <cmath>

#include "bars.hpp"
#include "border.hpp"
#include "enemyBullet.hpp"
#include "engine/entity.hpp"
#include "engine/world.hpp"
#include "healthManager.hpp"
#include "healthPack.hpp"
#include "include.h"

float Enemy::DefaultRadius = 30;
float Enemy::Speed = 4000;

float Enemy::droppedHealthHP = 1;

const enum Tags Enemy::tag = enemy;

/// Arguments: args == *Enemy
const std::string Enemy::spawnEvent = "enemySpawn";
/// Arguments: args == *Enemy
const std::string Enemy::deathEvent = "enemyDeath";

#define barDimensions (Vector2){DefaultRadius * 2, 10}

Enemy::Enemy(Vector2 pos)
  : Entity2D("Enemy", pos),
    healthManager(
      10,
      BarManager(
        &position,
        DefaultRadius * 1.5f,
        Bar(Vector2Zero(), barDimensions, RED, DARKGRAY, false)
      )
    ) {
    velocity = {0, 0};
    targetPos = {0, 0};
    radius = DefaultRadius;
    healthManager.getBar()->ShouldRender = true;
    colour = PINK;
    stateTime = 0;
    addTag(tag);
}

Enemy::~Enemy() {}

void Enemy::death() {
    World::callEvent(deathEvent, this);
    onDeath();
    if(healthManager.isDead())
        dropHealth();
    else
        valid = true;
}

void Enemy::init() {
    World::callEvent(spawnEvent, this);
    onSpawn();
}

void Enemy::process(float delta) {
    manageHealthBar(radius);
    position = position + velocity * delta;
    velocity = velocity * friction;
    stateTime += delta;
    manageStates(delta);
    Border::wrapEntity(this);
    healthManager.process(delta);
}

void Enemy::render() {
    // hmm what do we want for our enemies???
    // hear me out:
    // circle :3
    DrawCircleV(position, radius, colour);  // WHOOOOOO
    healthManager.render();
}

Vector2 Enemy::getShortestVectorToPlayer() const {
    return Border::getShortestPathToPoint(position, Player::get().position);
}

void Enemy::manageHealthBar(float r) {
    healthManager.targetDistance = r * 1.5f;
    if(healthManager.isDead()) killDefered();
}

Vector2 Enemy::getClosestPointToPlayerWithDistance(float dist) const {
    // normalize the offset
    Vector2 shortestPathToPlayer = getShortestVectorToPlayer();
    // lets get a model
    //--------->
    // say that is our vector to our player so our player is
    //--------->* here
    // i want a distance around there so we would have a vector of
    //<---
    // ish
    // so that's what we wanna do
    Vector2 vectorFromPlayer = Vector2Scale(Vector2Normalize(shortestPathToPlayer), -dist);
    Vector2 globalPos = Vector2Add(vectorFromPlayer, Player::get().position);
    Vector2 vectorToGlobal = Border::wrapPos(globalPos);
    // then we do some shmath
    return vectorToGlobal;
}

int Enemy::getState() const {
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

void ManageStates() {}

float Enemy::getStateTime() const {
    return stateTime;
}

HealthManager& Enemy::getHealthManager() {
    return healthManager;
}

void Enemy::resetStateTime() {
    stateTime = 0;
}

EnemyBullet* Enemy::fireBullet(float angle, float lifetime, Color col) const {
    EnemyBullet* bul = new EnemyBullet(position, angle, col, true, lifetime);
    World::addEntity(bul);
    return bul;
}

EnemyBullet* Enemy::fireBullet(float angle, float lifetime, float s, Color col) const {
    EnemyBullet* bul = new EnemyBullet(position, angle, col, true, lifetime, s);
    World::addEntity(bul);
    return bul;
}

EnemyBullet* Enemy::fireBullet(float angle, float lifetime, float s, float dmg, Color col) const {
    EnemyBullet* bul = new EnemyBullet(position, angle, col, true, lifetime, s, dmg);
    World::addEntity(bul);
    return bul;
}

Vector2 getHealthPackVel(Vector2 enVel) {
    float r = 2 * rand() / (float)RAND_MAX - 1;  // range = (-1, 1)
    float weight = r * r * r * r;                // r^4
    float aOfEn = atan2f(enVel.x, -enVel.y);     // find the angle of the enemies velocity
    float nT = weight * M_PI + aOfEn;            // add the random weighted angle to the
                                                 // actual angle of the velocity
    return (Vector2){cos(nT), sin(nT)};          // un-angle it w/ trig
}

// if not provided uses droppedHealthHP
// otherwise, same as the more verbose overload
void Enemy::dropHealthPack() {
    Vector2 v = getHealthPackVel(velocity);
    HealthPack* h = new HealthPack(position, velocity, droppedHealthHP);
    World::addEntity(h);
}

void Enemy::dropHealthPack(float hp) {
    Vector2 v = getHealthPackVel(velocity);
    HealthPack* h = new HealthPack(position, velocity, hp);
}

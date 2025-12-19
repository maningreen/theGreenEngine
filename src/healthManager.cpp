#include "healthManager.hpp"
#include "barManager.hpp"
#include <iostream>

#define MAX(a, b) (a < b ? b : a)

HealthManager::HealthManager(float maxHealth, BarManager b) : BarManager(b), maxHealth(maxHealth), health(maxHealth) {
  initBar();
}

HealthManager::HealthManager(float maxHealth, float health, BarManager b) : BarManager(b), maxHealth(maxHealth), health(health) {
  initBar();
}

void HealthManager::initBar() {
  Bar* bar = getBar(); //i HATE calling functions that much
  bar->Colour = RED;
  bar->EmptyCol = GRAY;
}

float HealthManager::getHealth() {
  return health;
}

void HealthManager::setHealth(float h) {
  if(h < 0)
    health = 0;
  else if(h > maxHealth)
    health = maxHealth;
  else
    health = h;
}

void HealthManager::applyDamage(float d) {
  setHealth(health - d);
}

void HealthManager::applyHealing(float h) {
  setHealth(health + h);
}

bool HealthManager::isDead() {
  return health <= 0;
}

float HealthManager::getMaxHealth() {
  return maxHealth;
}

void HealthManager::setMaxHealth(float m) {
  maxHealth = MAX(m, 0);
  setHealth(health);
}

HealthManager::~HealthManager() {}

void HealthManager::process(float delta) {
  setBarPercentage(health / maxHealth);
  getBar()->process(delta);
  if(positionPointer== nullptr) {
    return;
  }
  // now we know we gots to set our position
  getBar()->position = 
    *positionPointer +
      (!getBar()->ShrinkY 
        ? (Vector2){0, targetDistance} 
        : (Vector2){targetDistance, 0}) - 
    (!getBar()->ShrinkY 
      ? (Vector2){getBar()->Dimensions.x / 2.0f, 0}
      : (Vector2){0, getBar()->Dimensions.y / 2.0f}
    ); 
}

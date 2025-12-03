#include "healthManager.hpp"
#include "barManager.hpp"
#include <iostream>

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
  if(m >= 0)
    maxHealth = m;
  else
    maxHealth = 0;
  setHealth(health);
}

HealthManager::~HealthManager() {}

void HealthManager::process(float delta) {
  setBarPercentage(health / maxHealth);
  getBar()->process(delta);
  if(!getPositionPointer()) //should be 0 if not real
    return;
  // now we know we gots to set our position
  getBar()->position = Vector2Add(*getPositionPointer(), //then we want to offset it say... down/right depending on if it shrinks on it's y or not
      getBar()->ShrinkY ? (Vector2){90, 0} : (Vector2){0, targetDistance});
  getBar()->position = Vector2Subtract(Vector2Add(*getPositionPointer(), !getBar()->ShrinkY ? (Vector2){0, targetDistance} : (Vector2){targetDistance, 0}), !getBar()->ShrinkY ? (Vector2){ getBar()->Dimensions.x / 2.0f, 0} : (Vector2){0, getBar()->Dimensions.y / 2.0f});
}

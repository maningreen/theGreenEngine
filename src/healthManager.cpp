#include "healthManager.hpp"
#include "barManager.hpp"

HealthManager::HealthManager(float maxHealth, BarManager b) : BarManager(b), maxHealth(maxHealth), health(maxHealth) {}

HealthManager::HealthManager(float maxHealth, float health, BarManager b) : BarManager(b), maxHealth(maxHealth), health(health) {}

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

bool HealthManager::isDead() {
  return health <= 0;
}

float HealthManager::getMaxHealth() {
  return maxHealth;
}

void HealthManager::setMaxHealth(float m) {
  if(m < 0)
    maxHealth = 0;
  else {
    maxHealth = m;
    if(health > maxHealth)
      health = maxHealth;
  }
}

HealthManager::~HealthManager() {}

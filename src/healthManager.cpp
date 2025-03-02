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
}

HealthManager::~HealthManager() {}

void HealthManager::Process(float delta) {
  if(!getPositionPointer()) //should be 0 if not real
    return;
  // now we know we gots to set our position
  Position = Vector2Subtract(Vector2Add(*getPositionPointer(), !getBar()->ShrinkY ? (Vector2){0, targetDistance} : (Vector2){targetDistance, 0}), !getBar()->ShrinkY ? (Vector2){ getBar()->Dimensions.x / 2.0f, 0} : (Vector2){0, getBar()->Dimensions.y / 2.0f});
  getBar()->Position = Position;
  setBarPercentage(health / maxHealth);
  getBar()->ShouldRender = true;
}

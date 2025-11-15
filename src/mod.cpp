#include "mod.hpp"
#include "engine/entity.hpp"
#include "nodeBullet.hpp"

Mod::Mod(
  std::function<void(Entity2D*)> onInit,
  std::function<void(Entity2D*)> onDash,
  std::function<void(Entity2D*, NodeBullet*)> onFire,
  std::function<void(Entity2D*, Enemy*)> onEnemyKill,
  std::function<void(Entity2D*, Enemy*)> onEnemySpawn 
) : 
  onInit(onInit), 
  onDash(onDash), 
  onFire(onFire),
  onEnemyKill(onEnemyKill),
  onEnemySpawn(onEnemySpawn) {}

Mod::Mod(std::function<void(Entity2D*)> onInit) : onInit(onInit) {
  onDash = [](Entity2D* p){ return; };
  onFire = [](Entity2D* p, NodeBullet* b){ return; };
  onEnemyKill = [](Entity2D* p, Enemy* en){ return; };
  onEnemySpawn = [](Entity2D* p, Enemy* en){ return; };
}

Mod::~Mod() {}

ModManager::ModManager() {}

ModManager::~ModManager() {}

void ModManager::onDash(Entity2D* x) {
  for(Mod* m = mods.data(); m < mods.data() + mods.size(); (m++)->onDash(x));
}

void ModManager::onFire(Entity2D* x, NodeBullet* y) {
  for(Mod* m = mods.data(); m < mods.data() + mods.size(); (m++)->onFire(x, y));
}

void ModManager::onEnemyKill(Entity2D* x, Enemy* y) {
  for(Mod* m = mods.data(); m < mods.data() + mods.size(); (m++)->onEnemyKill(x, y));
}

void ModManager::onEnemySpawn(Entity2D* x, Enemy* y) {
  for(Mod* m = mods.data(); m < mods.data() + mods.size(); (m++)->onEnemySpawn(x, y));
}

void ModManager::addModPartial(Mod& x) {
  mods.push_back(x);
}

void ModManager::addMod(Mod& x, Entity2D* y) {
  x.onInit(y);
  mods.push_back(x);
}

void ModManager::removeMod(int i) {
  mods.erase(mods.begin() + i);
}

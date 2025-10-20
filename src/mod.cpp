#include "mod.hpp"

Mod::Mod(
    std::function<void(Entity2D*)> onInit,
    std::function<void(Entity2D*)> onDash,
    std::function<void(Entity2D*, NodeBullet*)> onFire,
    std::function<void(Entity2D*, Enemy*)> onEnemyKill,
    std::function<void(Entity2D*, Enemy*)> onEnemySpawn)
    : 
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

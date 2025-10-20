#ifndef mod
#define mod

#include "nodeBullet.hpp"
#include "enemy.hpp"
#include <functional>

class Mod {
  public:
    std::function<void(Entity2D*)> onInit;
    std::function<void(Entity2D*)> onDash;
    std::function<void(Entity2D*, NodeBullet*)> onFire;
    std::function<void(Entity2D*, Enemy*)> onEnemyKill;
    std::function<void(Entity2D*, Enemy*)> onEnemySpawn;

    Mod(
      std::function<void(Entity2D*)> onInit,
      std::function<void(Entity2D*)> onDash,
      std::function<void(Entity2D*, NodeBullet*)> onFire,
      std::function<void(Entity2D*, Enemy*)> onEnemyKill,
      std::function<void(Entity2D*, Enemy*)> onEnemySpawn
    );
    Mod(
      std::function<void(Entity2D*)> onInit
    );
    ~Mod();
};

#endif

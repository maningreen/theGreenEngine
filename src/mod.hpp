#ifndef mod
#define mod

#include "nodeBullet.hpp"
#include "enemy.hpp"
#include <functional>

struct Mod {
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

class ModManager {
  private:

    std::vector<Mod> mods;

  public:
    ModManager();
    ~ModManager();

    // this function calls `onInit` on all the mods
    void onInit(Entity2D*);

    // this function calls `onDash` on all the mods
    void onDash(Entity2D*);

    // this function calls `onFire` on all the mods
    void onFire(Entity2D*, NodeBullet*);

    // this function calls `onEnemyKill` on all the mods
    void onEnemyKill(Entity2D, Enemy*);
    // this function calls `onEnemySpawn` on all the mods
    void onEnemySpawn(Entity2D*, Enemy*);

    // adds a mod
    void addMod(Mod);
};

#endif

#ifndef mod
#define mod

#include "nodeBullet.hpp"
#include "enemy.hpp"
#include "engine/entity.hpp"
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

    // this function calls `onDash` on all the mods
    void onDash(Entity2D*);

    // this function calls `onFire` on all the mods
    void onFire(Entity2D*, NodeBullet*);

    // this function calls `onEnemyKill` on all the mods
    void onEnemyKill(Entity2D*, Enemy*);
    // this function calls `onEnemySpawn` on all the mods
    void onEnemySpawn(Entity2D*, Enemy*);

    // adds a mod, does *not* call onInit
    void addModPartial(Mod&);

    // adds a mod, calls onInit
    void addMod(Mod&, Entity2D*);

    // removes a mod.
    void removeMod(int i);
};

#endif

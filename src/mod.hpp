#ifndef mod_H
#define mod_H

#include "nodeBullet.hpp"
#include "enemy.hpp"
#include "engine/entity.hpp"
#include <functional>
#include <optional>
#include <sol/forward.hpp>
#include <sol/sol.hpp>

struct Mod {
  sol::function onInit;
  sol::function onDash;
  sol::function onFire;
  sol::function onEnemyKill;
  sol::function onEnemySpawn;

  Mod(sol::function onInit);

  // will return std::nullopt when there is no onInit
  static std::optional<Mod> fromTable(sol::table);

  ~Mod();
};

class ModManager {
  private:
    sol::state lua;

    // this function doesn't call onInit, only sets up the lua
    void initLua();
  public:
    std::vector<Mod> mods;

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
    void addModPartial(Mod);

    // adds a mod, calls onInit
    void addMod(Mod, Entity2D*);

    // loads all the mods in resources/mods
    // parses the lua and calls onInit
    void loadMods(Entity2D*);

    // removes a mod.
    void removeMod(int i);

    sol::state& getLua();
};

#endif

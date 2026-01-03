#ifndef mod_H
#define mod_H

#include "nodeBullet.hpp"
#include "enemy.hpp"
#include "engine/entity.hpp"
#include <functional>
#include <optional>
#include <list>
#include <sol/forward.hpp>
#include <sol/sol.hpp>

namespace mod {
  const std::string poolPath = "resources/mods/pool";
  const std::string initPath = "resources/mods/init";
};

struct Mod {
  std::string name;
  std::string description;

  std::optional<sol::function> onInit;
  std::optional<sol::function> onDash;
  std::optional<sol::function> onFire;
  std::optional<sol::function> onEnemyKill;
  std::optional<sol::function> onEnemySpawn;

  Mod(std::string name, sol::function onInit);
  Mod(std::string name);
  static std::optional<Mod> fromPath(fs::path path);

  // searches for a mod names so in `pool`
  static std::optional<Mod> fromName(std::string name);

  // will return std::nullopt when there is no onInit
  static Mod fromTable(std::string name, sol::table);

  ~Mod();
};

class ModManager {
  private:
    static sol::state lua;

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

    // loads a mod from `pool`
    // if fails, returns 0, otherwise 1
    // expects format: (poolPath)/(mod).lua
    int loadMod(std::string mod, Entity2D* plr);

    // lists all possible mods to be loaded
    std::list<std::string> listPoolMods();
    // picks a random pool mod to be loaded
    std::string pickRandomPoolMod();

    // removes a mod.
    void removeMod(int i);

    static sol::state& getLua();
};

#endif

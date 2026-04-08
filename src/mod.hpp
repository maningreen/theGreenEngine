#ifndef mod_H
#define mod_H

#include <functional>
#include <list>
#include <optional>
#include <sol/forward.hpp>
#include <sol/sol.hpp>

#include "enemy.hpp"
#include "engine/entity.hpp"
#include "nodeBullet.hpp"

namespace mod {
const fs::path poolPath = "resources/mods/pool";
const fs::path initPath = "resources/mods/init";
};  // namespace mod

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
    sol::state lua;

    void initLua();

  public:
    ModManager();
    ~ModManager();

    std::vector<Mod> mods;

    // this function calls `onDash` on all the mods
    void onDash(unsigned playerId);

    // this function calls `onFire` on all the mods
    void onFire(unsigned playerId, unsigned bulletId);

    // this function calls `onEnemyKill` on all the mods
    void onEnemyKill(unsigned playerId, unsigned enemyId);
    // this function calls `onEnemySpawn` on all the mods
    void onEnemySpawn(unsigned playerId, unsigned enemyId);

    // adds a mod, calls onInit
    void addMod(Mod, unsigned playerId);

    // loads all the mods in resources/mods/pool
    // parses the lua and calls onInit
    void loadMods(unsigned playerId);

    // loads a mod from `pool`
    // if fails, returns 1, otherwise 0
    // expects format: (poolPath)/(mod).lua
    int loadMod(std::string mod, unsigned playerId);

    // loads a mod from the specified path.
    int loadModPath(fs::path, unsigned playerId);

    std::optional<Mod> fromPath(fs::path);
    std::optional<Mod> fromName(std::string);

    // lists all possible mods to be loaded
    static std::list<std::string> listPoolMods();
    // picks a random pool mod to be loaded
    static std::string pickRandomPoolMod();

    // removes a mod.
    void removeMod(int i);

    sol::state& getLua();
};

#endif

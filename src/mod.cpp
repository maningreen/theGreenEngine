#include <sol/load_result.hpp>
#define SOL_ALL_SAFETIES_ON 1
#include "mod.hpp"
#include "enemy.hpp"
#include "engine/entity.hpp"
#include "nodeBullet.hpp"
#include "player.hpp"
#include <sol/forward.hpp>
#include <sol/resolve.hpp>
#include <sol/sol.hpp>
#include <sol/state_view.hpp>

// NOTE: all Entity2D*'s in this struct are expected to be Player*s
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
  onEnemySpawn(onEnemySpawn)
  {}

Mod::Mod(std::function<void(Entity2D*)> onInit) : onInit(onInit) {
  onDash = [](Entity2D* p){ };
  onFire = [](Entity2D* p, NodeBullet* b){ };
  onEnemyKill = [](Entity2D* p, Enemy* en){ };
  onEnemySpawn = [](Entity2D* p, Enemy* en){ };
}

Mod::~Mod() {}

ModManager::ModManager() {
  initLua();
}

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

sol::state& ModManager::getLua() {
  return lua;
}

// got bless macros

#define MODTHING(var, func) if(func.valid()) var.func = func
#define MODTHING2(var, mod, funcName, search)           \
  {                                                     \
    sol::function funcName = var[search];               \
    if(funcName.valid()) mod.funcName = funcName;       \
  }

void ModManager::initLua() {
  lua.open_libraries();

  for(const fs::directory_entry& p : fs::directory_iterator("resources/mods")) {
    sol::load_result chunk = lua.load_file(p.path().string());
    sol::table mod = chunk();

    sol::function onInit = mod["onInit"];

    Mod x(onInit);
    MODTHING2(mod, x, onDash, "onDash")
    MODTHING2(mod, x, onFire, "onFire")
    MODTHING2(mod, x, onEnemyKill, "onKill")
    MODTHING2(mod, x, onEnemySpawn, "onSpawn")
    addMod(x, nullptr);
  }
}

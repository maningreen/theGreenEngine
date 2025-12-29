#include "customEnemy.hpp"
#include "dasher.hpp"
#include "enemy.hpp"
#include "engine/entity.hpp"
#include "mod.hpp"
#include "player.hpp"
#include "border.hpp"
#include "player.hpp"
#include "sniper.hpp"
#include "spiral.hpp"
#include <optional>
#include <sol/forward.hpp>

sol::state ModManager::lua;

ModManager::ModManager() : mods({}) {
  initLua();
}

ModManager::~ModManager() {}

void ModManager::onDash(Entity2D* x) {
  for(Mod& x : mods)
    if(x.onDash.has_value() && x.onDash.value())
      x.onDash.value()(x);
}

void ModManager::onFire(Entity2D* x, NodeBullet* y) {
  for(Mod& m : mods)
    if(m.onFire.has_value() && m.onFire.value())
      m.onFire.value()((Player*)x, y);
}

void ModManager::onEnemyKill(Entity2D* x, Enemy* y) {
  for(Mod* m = mods.data(); m < mods.data() + mods.size(); m++)
    if(m->onEnemyKill.has_value() && m->onEnemyKill.value()) 
      m->onEnemyKill.value()((Player*)x, y);
}

void ModManager::onEnemySpawn(Entity2D* x, Enemy* y) {
  for(Mod& m : mods)
    if(m.onEnemySpawn.has_value() && m.onEnemySpawn.value())
      (m.onEnemySpawn.value())((Player*)x, y);
}

void ModManager::addModPartial(Mod x) {
  mods.push_back(x);
}

void ModManager::addMod(Mod x, Entity2D* y) {
  if(!x.onInit.has_value())
    mods.push_back(x);
  else {
    sol::function_result z = x.onInit.value()((Player*)y);
    if(!z.valid() || (int)z != 1) // didn't return anything, carry on || didn't return anything we care about
      mods.push_back(x);
  }
  mods.push_back(x);
}

void ModManager::removeMod(int i) {
}

sol::state& ModManager::getLua() {
  return lua;
}

#define MOD(search, set) \
  sol::function set = result[search]; \
  if(set.valid())      \
    mod.set = set;     \
  else                 \
    mod.set = std::nullopt;

void ModManager::loadMods(Entity2D* plr) {
  for(const fs::directory_entry& p : fs::directory_iterator(mod::initPath)) {
    sol::table result = lua.script_file(p.path().string());
    if(!result.valid()) continue;
    Mod mod = Mod(p.path().filename().string());
    MOD("onInit", onInit);
    MOD("onKill", onEnemyKill);
    MOD("onSpawn", onEnemySpawn);
    MOD("onDash", onDash);
    MOD("onFire", onFire);
    addMod(mod, plr);
  }
}

#undef MOD

int ModManager::loadMod(std::string name, Entity2D* plr) {
  // check if file is valid, if so we return 1
  std::string path = mod::poolPath;
  path.append(1, '/').append(name).append(".lua"); // we assume it fits in the format, with a .lua extention
  if(!fs::exists(path))
    return 1;

  // great, we continue
  sol::table modTable = lua.script_file(path); // load the file
  if(!modTable.valid()) return 1;
  Mod mod = Mod::fromTable(name, modTable); // parse the table
  addMod(mod, plr); // if it's valid shablamo

  return 0;
}

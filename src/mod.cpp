#include "dashManager.hpp"
#include "healthManager.hpp"
#include "include.h"
#include "inputManager.hpp"
#include <cstdio>
#include <sol/forward.hpp>
#include "mod.hpp"
#include "enemy.hpp"
#include "engine/entity.hpp"
#include "nodeBullet.hpp"
#include "player.hpp"
#define SOL_ALL_SAFETIES_ON 1
#include <sol/sol.hpp>

Mod::Mod(sol::function init) {
  onInit = init;
}

#define MODTHING(var, table, functionName, key) if(sol::function function = table[key]) var.functionName = function; else var.functionName = std::nullopt

std::optional<Mod> Mod::fromTable(sol::table table) {
  sol::function onInit = table["onInit"];
  if(!onInit.valid())
    return std::nullopt;
  Mod x(onInit);

  MODTHING(x, table, onDash, "onDash");
  MODTHING(x, table, onFire, "onFire");
  MODTHING(x, table, onEnemyKill, "onKill");
  MODTHING(x, table, onEnemySpawn, "onSpawn");

  return x;
}

#undef MODTHING

Mod::~Mod() {}

ModManager::ModManager() {
  initLua();
}

ModManager::~ModManager() {}

void ModManager::onDash(Entity2D* x) {
  for(Mod* m = mods.data(); m < mods.data() + mods.size(); m++) {
    if(m->onDash)
      m->onDash.value()((Player*)x);
  }
}

void ModManager::onFire(Entity2D* x, NodeBullet* y) {
  for(Mod* m = mods.data(); m < mods.data() + mods.size(); m++) {
    if(m->onFire)
    m->onFire.value()((Player*)x, y);
  }
}

void ModManager::onEnemyKill(Entity2D* x, Enemy* y) {
  for(Mod* m = mods.data(); m < mods.data() + mods.size(); m++) {
    if(m->onEnemyKill)
      m->onEnemyKill.value()((Player*)x, y);
  }
}

void ModManager::onEnemySpawn(Entity2D* x, Enemy* y) {
  for(Mod* m = mods.data(); m < mods.data() + mods.size(); m++) {
    if(m->onEnemySpawn)
      (m->onEnemySpawn.value())((Player*)x, y);
  }
}

void ModManager::addModPartial(Mod x) {
  mods.push_back(x);
}

void ModManager::addMod(Mod x, Entity2D* y) {
  x.onInit((Player*)y);
  mods.push_back(x);
}

void ModManager::removeMod(int i) {
  mods.erase(mods.begin() + i);
}

sol::state& ModManager::getLua() {
  return lua;
}

void ModManager::initLua() {
  lua.open_libraries();

  sol::usertype<Vector2> vec = lua.new_usertype<Vector2>(
    "vector2"
  );
  vec["x"] = &Vector2::x;
  vec["y"] = &Vector2::y;
  vec["normalize"] = &Vector2Normalize;
  vec["scale"] = &Vector2Scale;
  lua["vector"] = [](float a, float b){ return (Vector2){a, b}; };

  sol::usertype<Player> plr = lua.new_usertype<Player>(
    "Player",
    sol::constructors<Player(std::string&, Vector2)>()
  // , "defaultMaxHealth", &Player::maxHealth
  // , "defaultDashSpeed", &Player::defaultDashSpeed
  // , "defaultDashTime", &Player::defaultDashTime
  // , "defaultDashControl", &Player::defaultDashControl
  // , "defaultDashRegenDelay", &Player::defaultDashRegenDelay
  // , "defaultMaxDashCount", &Player::defaultMaxDashCount
  // , "defaultDashCooldown", &Player::defaultDashCooldown
  // , "particleSpawnTime", &Player::particleSpawnTime
  );
  plr["getHealthManager"] = &Player::getHealthManager;
  plr["getInputManager"] = &Player::getInputManager;
  plr[ "getDashManager"] = &Player::getDashManager;
  plr["position"] = &Player::Position;
  plr["velocity"] = &Player::velocity;
  plr["speed"] = &Player::speed;
  plr["friction"] = &Player::friction;
  plr["fireBullet"] = &Player::fireBullet;
  plr["test"] = [](Player x){
    printf("GOD DAMN");
  };

  sol::usertype<HealthManager> hm = lua.new_usertype<HealthManager>(
    "healthManager"
  );
  hm["getHealth"] = &HealthManager::getHealth;
  hm["setHealth"] = &HealthManager::setHealth;
  hm["applyDamage"] = &HealthManager::applyDamage;
  hm["applyHealing"] = &HealthManager::applyHealing;
  hm["isDead"] = &HealthManager::isDead;
  hm["getMaxHealth"] = &HealthManager::getMaxHealth;
  hm["setMaxHealth"] = &HealthManager::setMaxHealth;

  sol::usertype<DashManager> dm = lua.new_usertype<DashManager>(
    "dashManager"
  );
  dm["dashSpeed"] = &DashManager::dashSpeed;
  dm["dashControl"] = &DashManager::dashControl;
  dm["dashRegenDelay"] = &DashManager::dashRegenDelay;
  dm["dashLength"] = &DashManager::dashLength;
  dm["maxDashCount"] = &DashManager::maxDashCount;
  dm["getAvailableDashes"] = &DashManager::getAvailableDashes;
  dm["isDashing"] = &DashManager::isDashing;
  dm["getDashProgress"] = &DashManager::getDashProgress;
  dm["removeDashProgress"] = &DashManager::removeDashProgress;
  dm["addDash"] = &DashManager::addDashProgress;
  dm["getDashVelocity"] = &DashManager::getDashVelocity;
  dm["getDashDirection"] = &DashManager::getDashDirection;
  dm["getDeltaDash"] = &DashManager::getDeltaDash;
  dm["canDash"] = &DashManager::canDash;

  lua.new_usertype<keybind>(
    "keybind", sol::constructors<keybind(Key, bool, sol::function)>()
  );

  lua.new_usertype<keybindAlt>(
    "keybind", sol::constructors<keybind(Key, bool, sol::function)>()
  );

sol::usertype<InputManager> im =  lua.new_usertype<InputManager>(
    "inputManager"
  );
  im["addVectorBind"] = &InputManager::addVectorBind;
  im["addBind"] = sol::overload([](InputManager* self, const keybind& b) { self->addBind(b); }, [](InputManager* self, const keybindAlt& b) { self->addBind(b); });
  im["up"] = &InputManager::up;
  im["down"] = &InputManager::down;
  im["left"] = &InputManager::left;
  im["right"] = &InputManager::right;
  im["removeVectorBind"] = &InputManager::removeVectorBind;
  im["removeBind"] = &InputManager::removeBind;
}

// got bless macros
#define SETMODFUNCTION(var, mod, funcName, search) \
  if(sol::function funcName = var[search]) \
    mod.funcName(funcName)

void ModManager::loadMods(Entity2D* plr) {
  for(const fs::directory_entry& p : fs::directory_iterator("resources/mods")) {
    sol::table modTable = lua.script_file(p.path().string());
    auto mod = Mod::fromTable(modTable);
    if(mod.has_value())
      addMod(mod.value(), plr);
  }
}

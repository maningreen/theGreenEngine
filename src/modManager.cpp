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

struct maybe {
  void* value;
  bool valid;

  maybe() {
    valid = false;
    value = nullptr;
  }

  template<typename T>
  maybe(T x) {
    value = new T;
    *(T*)value = x;
  }

  maybe(void* x) {
    value = x;
  }

  ~maybe() {
    if(valid)
      delete (int*)value;
  }
};

sol::state ModManager::lua;

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
  sol::function_result z = x.onInit((Player*)y);
  if(!z.valid() || (int)z != 1) // didn't return anything, carry on || didn't return anything we care about
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
  vec["add"] = &Vector2Add;
  vec["subtract"] = &Vector2Subtract;
  vec["scale"] = &Vector2Scale;
  vec["lerp"] = &Vector2Lerp;
  lua["vector2"] = [](float a, float b){ return (Vector2){a, b}; };

  sol::usertype<Color> col = lua.new_usertype<Color>("color");
  col["r"] = &Color::r;
  col["g"] = &Color::g;
  col["b"] = &Color::b;
  col["a"] = &Color::a;
  col["lerp"] = &ColorLerp;
  lua["color"] = [](unsigned char r, unsigned char g, unsigned char b, unsigned char a){
    return (Color){r, g, b, a};
  };

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
  plr["getHealth"] = &Player::getHealthManager;
  plr["getInput"] = &Player::getInputManager;
  plr["getDash"] = &Player::getDashManager;
  plr["position"] = &Player::position;
  plr["velocity"] = &Player::velocity;
  plr["speed"] = &Player::speed;
  plr["fireBullet"] = &Player::fireBullet;

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
  dm["speed"] = &DashManager::speed;
  dm["control"] = &DashManager::control;
  dm["regenDelay"] = &DashManager::regenDelay;
  dm["regenRate"] = &DashManager::regenRate;
  dm["length"] = &DashManager::length;
  dm["maxDashCount"] = &DashManager::maxDashCount;
  dm["getAvailableDashes"] = &DashManager::getAvailableDashes;
  dm["isDashing"] = &DashManager::isDashing;
  dm["getDashProgress"] = &DashManager::getDashProgress;
  dm["removeDash"] = &DashManager::removeDashProgress;
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

  sol::usertype<InputManager> im = lua.new_usertype<InputManager>(
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

   sol::usertype<Enemy> en = lua.new_usertype<Enemy>(
    "enemy",
    sol::constructors<Enemy(Vector2)>()
  );

  sol::table global = lua["Global"].get_or_create<sol::table>();
  global.set_function("getFriction", [](){ return Entity2D::friction; });
  global.set_function("setFriction", [](float f){ Entity2D::friction = f; });

  en["position"] = &Enemy::position;
  en["velocity"] = &Enemy::velocity;
  en["radius"] = &Enemy::radius;
  en["getStateTime"] = &Enemy::getStateTime;
  en["resetStateTime"] = &Enemy::resetStateTime;
  en["setState"] = &Enemy::setState;
  en["getState"] = &Enemy::getState;
  en["getHealth"] = &Enemy::getHealthManager;
  en["dropHealthPack"] = sol::overload(
    [](Enemy* self){
      self->dropHealthPack();
    },
    [](Enemy* self, float hp){
      self->dropHealthPack(hp);
    }
  );
  en["color"] = &Enemy::colour;

  sol::table enemy = lua["Enemy"].get_or_create<sol::table>();
  enemy.set_function(
    "addDeathHook",
    &Enemy::addDeathHook
  );
  enemy.set_function(
    "addSpawnHook",
    &Enemy::addSpawnHook
  );
  enemy.set_function(
    "spawnEnemy",
    [](Vector2 x){
      Enemy* y = new Enemy(x); 
      Entity::getRoot()->addChild(y);
      return y;
    }
  );
  enemy.set_function(
    "spawnSpiraler",
    [](Vector2 x){
      Spiraler* y = new Spiraler(x); 
      Entity::getRoot()->addChild(y);
      return y;
    }
  );
  enemy.set_function(
    "spawnDasher",
    [](Vector2 x){
      Dasher* y = new Dasher(x); 
      Entity::getRoot()->addChild(y);
      return y;
    }
  );
  enemy.set_function(
    "spawnSniper",
    [](Vector2 x){
      Sniper* y = new Sniper(x); 
      Entity::getRoot()->addChild(y);
      return y;
    }
  );
  // enemy.set_function(
    // ""
  // )

  sol::usertype<NodeBullet> nb = lua.new_usertype<NodeBullet>("nodeBullet");
  nb["theta"] = &NodeBullet::theta;
  nb["lifetime"] = &NodeBullet::lifetime;
  nb["targetLifetime"] = &NodeBullet::targetLifetime;

  sol::usertype<maybe> mb = lua.new_usertype<maybe>("maybe");
  mb["valid"] = &maybe::valid;
  mb["value"] = &maybe::value;

  sol::table maybeTable = lua["Maybe"].get_or_create<sol::table>();
  mb["just"] = [](void* x){
    return maybe(x);
  };
  mb["nothing"] = [](){
    return maybe();
  };

  sol::table nodeBullet = lua["NodeBullet"].get_or_create<sol::table>();
  nodeBullet["speed"] = &NodeBullet::speed;
  nodeBullet["radius"] = &NodeBullet::radius;
  nodeBullet["color"] = &NodeBullet::color;

  sol::table border = lua["Border"].get_or_create<sol::table>();
  border["length"] = Border::length;
  border["wrapEntity"] = &Border::wrapEntity;
  border["wrapPos"] = &Border::wrapPos;
  border["wrapPosX"] = &Border::wrapPosX;
  border["wrapPosY"] = &Border::wrapPosY;
  border["getShortestPathToPoint"] = sol::overload([](Vector2 a, Vector2 b){
    return Border::getShortestPathToPoint(a, b);
  });
  border["getDistance"] = &Border::getDistance;

  sol::table customEnemy = lua["CustomEnemy"].get_or_create<sol::table>();
  customEnemy["addCustomEnemy"] = &CustomEnemy::addCustomEnemy;
  customEnemy["spawnEnemy"] = [](std::string name, Vector2 p){
    std::optional<CustomEnemy*> ret = CustomEnemy::spawnEnemy(name);
    if(ret.has_value())
      return maybe((void*)ret.value());
    else 
      return maybe();
  };
}

void ModManager::loadMods(Entity2D* plr) {
  for(const fs::directory_entry& p : fs::directory_iterator(mod::initPath)) {
    sol::table modTable = lua.script_file(p.path().string());
    auto mod = Mod::fromTable(p.path().filename().string(), modTable);
    if(mod.has_value())
      addMod(mod.value(), plr);
  }
}

int ModManager::loadMod(std::string name, Entity2D* plr) {
  // check if file is valid, if so we return 1
  std::string path = mod::poolPath;
  path.append(1, '/').append(name).append(".lua"); // we assume it fits in the format, with a .lua extention
  if(!fs::exists(path))
    return 1;

  // great, we continue
  sol::table modTable = lua.script_file(path); // load the file
  auto mod = Mod::fromTable(name, modTable); // parse the table
  if(mod.has_value()) // check validity
    addMod(mod.value(), plr); // if it's valid shablamo

  return 0;
}

#include "mod.hpp"
#include "customEnemy.hpp"
#include "player.hpp"
#include "enemy.hpp"
#include "raylib.h"
#include "border.hpp"
#include "sniper.hpp"

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
  vec["length"] = &Vector2Length;
  vec["lengthSqr"] = &Vector2LengthSqr;
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
  // global.set_function("getPlayer", [](){ return Player::get(); });

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
  enemy["addDeathHook"] = &Enemy::addDeathHook;
  enemy["addSpawnHook"] = &Enemy::addSpawnHook;
  enemy[ "spawnEnemy"] = [](Vector2 x){
    Enemy* y = new Enemy(x); 
    Entity::getRoot()->addChild(y);
    return y;
  };
  enemy[ "spawnSniper"] = [](Vector2 x){
    Sniper* y = new Sniper(x); 
    Entity::getRoot()->addChild(y);
    return y;
  };

  sol::usertype<NodeBullet> nb = lua.new_usertype<NodeBullet>("nodeBullet");
  nb["theta"] = &NodeBullet::theta;
  nb["lifetime"] = &NodeBullet::lifetime;
  nb["targetLifetime"] = &NodeBullet::targetLifetime;

  sol::table nodeBullet = lua["NodeBullet"].get_or_create<sol::table>();
  nodeBullet["speed"] = &NodeBullet::speed;
  nodeBullet["radius"] = &NodeBullet::radius;
  nodeBullet["color"] = &NodeBullet::color;

  sol::table border = lua["Border"].get_or_create<sol::table>();
  border["setLength"] = [](float l){ Border::length = abs(l); };
  border["getLength"] = [](){ return Border::length; };
  border["wrapEntity"] = &Border::wrapEntity;
  border["wrapPos"] = &Border::wrapPos;
  border["wrapPosX"] = &Border::wrapPosX;
  border["wrapPosY"] = &Border::wrapPosY;
  border["getShortestPathToPoint"] = [](const Vector2 a, const Vector2 b){ return Border::getShortestPathToPoint(a, b); };
  border["getDistance"] = &Border::getDistance;
  border["getRandomPoint"] = &Border::getRandomPosInBorder;

  sol::table customEnTable = lua["CustomEnemy"].get_or_create<sol::table>();
  customEnTable["addEnemy"] = &CustomEnemy::addCustomEnemy;
  customEnTable["spawnEnemy"] = &CustomEnemy::spawnEnemy;

  sol::table plrTbl = lua["Player"].get_or_create<sol::table>();
  plrTbl["hitBoxRadius"] = Player::hitboxRadius;

  sol::usertype<CustomEnemy> customEn = lua.new_usertype<CustomEnemy>("customEnemy");
  customEn["position"] = &CustomEnemy::position;
  customEn["velocity"] = &CustomEnemy::velocity;
  customEn["radius"] = &CustomEnemy::radius;
  customEn["data"] = &CustomEnemy::data;
  customEn["getStateTime"] = [](CustomEnemy* self){
    return self->getStateTime();
  };
  customEn["getClosestPointToPlayerWithDistance"] = &CustomEnemy::getClosestPointToPlayerWithDistance;
  customEn["resetStateTime"] = &CustomEnemy::resetStateTime;
  customEn["state"] = sol::property(&CustomEnemy::getState, &CustomEnemy::setState);
  customEn["getHealth"] = &CustomEnemy::getHealthManager;
  customEn["dropHealthPack"] = sol::overload(
    [](CustomEnemy* self){
      self->dropHealthPack();
    },
    [](CustomEnemy* self, float hp){
      self->dropHealthPack(hp);
    }
  );
  customEn["spawn"] = &CustomEnemy::onSpawnCustom;
  customEn["death"] = &CustomEnemy::onDeathCustom;
  customEn["manageState"] = &CustomEnemy::manageStateCustom;
  customEn["dropHealth"] = &CustomEnemy::dropHealthCustom;
  customEn["color"] = &CustomEnemy::colour;
  customEn["fireBullet"] = sol::overload(
    [](CustomEnemy& self, float a, float b, Color c){
      self.fireBullet(a, b, c);
    },
    [](CustomEnemy& self, float a, float b, float l, Color c){
      self.fireBullet(a, b, l, c);
    },
    [](CustomEnemy& self, float a, float b, float l, float d, Color c){
      self.fireBullet(a, b, l, d, c);
    }
  );
}

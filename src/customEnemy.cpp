#include "customEnemy.hpp"
#include "enemy.hpp"
#include "raylib.h"
#include <optional>
#include <sol/sol.hpp>
#include <unordered_map>

std::unordered_map<std::string, CustomEnemy> CustomEnemy::customEnemies;

CustomEnemy::CustomEnemy(
  Vector2 p,
  Color c,
  float r,
  int state, 
  std::string n,
  sol::function spawn,
  sol::function drop,
  sol::function manageState,
  sol::function death
) : Enemy(p) {
  name = n;
  colour = c;
  radius = r;
  setState(state);
  onDeathCustom = death;
  onSpawnCustom = spawn;
  manageStateCustom = manageState;
  onSpawnCustom = spawn;
}

void CustomEnemy::manageStates(float delta) {
  manageStateCustom(this, delta);
}

void CustomEnemy::onDeath() {
  onDeathCustom(this);
}

void CustomEnemy::dropHealth() {
  dropHealthCustom(this);
}

void CustomEnemy::onSpawn() {
  onSpawnCustom(this);
}

std::optional<CustomEnemy> CustomEnemy::fromTable(sol::table x) {
  auto p = x["position"];
  auto r = x["radius"];
  auto c = x["color"];
  auto s = x["initialState"];
  auto spawn = x["onSpawn"];
  auto name = x["name"];
  auto drop = x["dropHealth"];
  auto manageState = x["manageState"];
  auto death = x["onDeath"];
  if(!(p && r && name && s && c && spawn && drop && manageState && death))
    return std::nullopt;
  else return CustomEnemy(p, c, r, name, s, spawn, drop, manageState, death);
}

void CustomEnemy::addCustomEnemy(sol::table x) {
  std::optional<CustomEnemy> c = fromTable(x);
  if(!c) return;
  else customEnemies.emplace(c.value().name, c.value());
}

std::optional<CustomEnemy*> CustomEnemy::spawnEnemy(std::string name) {
  auto x = customEnemies.find(name);
  if(x == customEnemies.end())
    return std::nullopt;
  else {
    CustomEnemy* y = new CustomEnemy(x->second);
    getRoot()->addChild(y);
    return y;
  } 
}

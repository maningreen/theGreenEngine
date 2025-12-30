#include "customEnemy.hpp"
#include "enemy.hpp"
#include "raylib.h"
#include <optional>
#include <sol/sol.hpp>
#include <unordered_map>

std::unordered_map<std::string, CustomEnemy> CustomEnemy::customEnemies;

CustomEnemy::CustomEnemy(
  Color c,
  float r,
  int state, 
  std::string n,
  sol::function spawn,
  sol::function drop,
  sol::function manageState,
  sol::function death
) : Enemy(Vector2Zero()) {
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
  auto r = x["radius"];
  auto c = x["color"];
  auto s = x["initialState"];
  auto spawn = x["onSpawn"];
  std::optional<std::string> name = x["name"];
  auto drop = x["dropHealth"];
  auto state = x["manageState"];
  auto death = x["onDeath"];
  if(!(r && name && s && c && spawn && drop && state && death))
    return std::nullopt;
  else
    return CustomEnemy(c, r, s, name.value(), spawn, drop, state, death);
}

void CustomEnemy::addCustomEnemy(sol::table x) {
  std::optional<CustomEnemy> t = fromTable(x);
  if(t.has_value()) {
    CustomEnemy val = t.value();
    auto y = customEnemies.emplace(val.name, val);
    std::cout << "name: " << y.first->second.name << '\n';
  }
}

std::optional<CustomEnemy*> CustomEnemy::spawnEnemy(std::string name, Vector2 p) {
  auto x = customEnemies.find(name);
  if(x != customEnemies.end()) {
    CustomEnemy* y = new CustomEnemy(x->second);
    y->position = p;
    getRoot()->addChild(y);
    return y;
  } else
    return std::nullopt;
}

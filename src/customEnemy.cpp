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
  if(manageStateCustom.valid())
    manageStateCustom(this, delta);
}

void CustomEnemy::onDeath() {
  if(onDeathCustom.valid())
    onDeathCustom(this);
}

void CustomEnemy::dropHealth() {
  if(dropHealthCustom.valid())
    dropHealthCustom(this);
}

void CustomEnemy::onSpawn() {
  if(onSpawnCustom.valid())
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
  if(!(r && name && s && c && spawn && drop && state && death)) {
    if(!r.valid())
      std::cout << "missing radius\n";
    if(!name.has_value()) 
      std::cout << "missing name\n";
    if(!s.valid())
      std::cout << "missing initialState\n";
    if(!c.valid())
      std::cout << "missing color\n";
    if(!spawn.valid())
      std::cout << "missing onSpawn\n";
    if(!drop.valid())
      std::cout << "missing dropHealth\n";
    if(!state.valid())
      std::cout << "missing manageState\n";
    if(!death.valid())
      std::cout << "missing onDeath\n";
    return std::nullopt;
  }
  else
    return CustomEnemy(c, r, s, name.value(), spawn, drop, state, death);
}

void CustomEnemy::addCustomEnemy(sol::table x) {
  std::optional<CustomEnemy> t = fromTable(x);
  if(t.has_value()) {
    customEnemies.emplace(t.value().name, t.value());
  } else
    DEBUG;
}

std::optional<CustomEnemy*> CustomEnemy::spawnEnemy(std::string name, Vector2 p) {
  auto x = customEnemies.find(name);
  for(auto item : customEnemies) {
    if(item.first == name) {
      CustomEnemy* y = new CustomEnemy(item.second);
      y->position = p;
      getRoot()->addChild(y);
      return y;
    }
  }
  DEBUG;
  return std::nullopt;
}

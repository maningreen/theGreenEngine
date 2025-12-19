#include "customEnemy.hpp"
#include "enemy.hpp"
#include "raylib.h"
#include "raymath.h"
#include <optional>
#include <sol/sol.hpp>
#include <unordered_map>

std::unordered_map<std::string, std::optional<CustomEnemy>> CustomEnemy::customEnemies;

CustomEnemy::CustomEnemy(
  Color c,
  float r,
  int state, 
  float maxHealth,
  std::string n,
  sol::function spawn,
  sol::function drop,
  sol::function manageState,
  sol::function death
) : Enemy(Vector2Zero()) {
  getHealthManager()->setMaxHealth(maxHealth);
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
  getHealthManager()->positionPointer = &position; // TODO: fix :p
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

void CustomEnemy::fromTable(sol::table x) {
  float maxHealth = x["maxHealth"];
  float r = x["radius"];
  auto c = x["color"];
  int s = x["initialState"];
  std::string name = x["name"];
  sol::function spawn = x["onSpawn"];
  sol::function drop = x["dropHealth"];
  sol::function manageState = x["manageState"];
  sol::function death = x["onDeath"];
  if(r && s && c && spawn && drop && manageState && death) {
    std::cout << "Valid enemy!\n";
    std::cout << "added enemy type: " << (std::string)name << '\n';
    customEnemies[name] = CustomEnemy(
      c,
      r,
      s,
      maxHealth,
      name,
      spawn,
      drop,
      manageState,
      death
    );
  }
  else { 
    if(!r)
      std::cout << "Missing radius\n";
    if(!s)
      std::cout << "Missing state\n";
    if(!c)
      std::cout << "Missing colour\n";
    std::cout << "Invalid enemy!\n";
  }
}

std::optional<CustomEnemy*> CustomEnemy::spawnEnemy(std::string name, Vector2 p) {
  if(customEnemies.count(name) > 0) {
    std::optional<CustomEnemy> x = customEnemies[name];
    if(x.has_value()) {
      std::cout << "spawned enemy " << name << '\n';
      CustomEnemy* y = new CustomEnemy(x.value());
      y->position = p;
      getRoot()->addChild(y);
      return y;
    } else {
      std::cout << "invalid enemy type: " << name << '\n';
      return std::nullopt;
    }
  } else {
    std::cout << "invalid enemy type: " << name << '\n';
    return std::nullopt;
  } 
}

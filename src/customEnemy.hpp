#ifndef CUSTOM_EN_H
#define CUSTOM_EN_H

#include "enemy.hpp"
#include "raylib.h"
#include <optional>
#include <sol/forward.hpp>
#include <sol/sol.hpp>

struct CustomEnemy : public Enemy {
public:
  sol::object data;
  sol::function manageStateCustom;
  sol::function dropHealthCustom;
  sol::function onDeathCustom;
  sol::function onSpawnCustom;

  static std::unordered_map<std::string, CustomEnemy> customEnemies;

public:
  void manageStates(float delta) override;
  void dropHealth() override;
  void onDeath() override;
  void onSpawn() override;

  static void addCustomEnemy(sol::table);
  static std::optional<CustomEnemy> fromTable(sol::table);
  static std::optional<CustomEnemy*> spawnEnemy(std::string name);

  CustomEnemy(
    Vector2 position,
    Color color,
    float radius,
    int initialState,
    std::string name,
    sol::function spawn,
    sol::function drop,
    sol::function manageState,
    sol::function death
  );
};

#endif

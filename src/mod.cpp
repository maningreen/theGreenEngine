#include <filesystem>
#include <sol/forward.hpp>
#define SOL_ALL_SAFETIES_ON 1

#include <cstdio>
#include <string>
#include "mod.hpp"
#include <sol/sol.hpp>

Mod::Mod(std::string n, sol::function init) {
  name = n;
  onInit = init;
}

Mod::Mod(std::string n) {
  name = n;
  onInit = std::nullopt;
  onDash = std::nullopt;
  onFire = std::nullopt;
  onEnemyKill = std::nullopt;
  onEnemySpawn = std::nullopt;
}

std::optional<Mod> Mod::fromPath(fs::path x) {
  DEBUG;
  sol::function_result result = ModManager::getLua().script_file(x);
  DEBUG;
  return fromTable(x.filename().string(), result);
}

#define MODTHING(var, table, functionName, key)                                \
  {                                                                            \
    sol::function function = table[key];                                       \
    if (function.valid())                                                      \
      var.functionName = function;                                             \
    else                                                                       \
      var.functionName = std::nullopt;                                         \
  }

std::optional<Mod> Mod::fromTable(std::string n, sol::table table) {
  sol::function onInit = table["onInit"];
  if(!onInit.valid())
    return std::nullopt;
  Mod x(n, onInit);

  MODTHING(x, table, onDash, "onDash");
  MODTHING(x, table, onFire, "onFire");
  MODTHING(x, table, onEnemyKill, "onKill");
  MODTHING(x, table, onEnemySpawn, "onSpawn");

  return x;
}

#undef MODTHING

Mod::~Mod() {}

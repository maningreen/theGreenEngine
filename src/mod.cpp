#include <filesystem>
#define SOL_ALL_SAFETIES_ON 1
#include <sol/forward.hpp>

#include "mod.hpp"
#include <cstdio>
#include <sol/sol.hpp>
#include <string>

Mod::Mod(std::string n, sol::function init) {
  name = n;
  onInit = init;
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
  // sol::function onInit = table["onInit"];
  // if (!onInit.valid())
    // return std::nullopt;
  // Mod x(n, onInit);

  // MODTHING(x, table, onDash, "onDash")
  // MODTHING(x, table, onFire, "onFire")
  // MODTHING(x, table, onEnemyKill, "onKill")
  // MODTHING(x, table, onEnemySpawn, "onSpawn")
  // return x;
  return std::nullopt;
}

#undef MODTHING

Mod::~Mod() {}

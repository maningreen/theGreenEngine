#include <filesystem>
#define SOL_ALL_SAFETIES_ON 1
#include <cstdio>
#include <sol/forward.hpp>
#include <sol/sol.hpp>
#include <string>

#include "mod.hpp"

Mod::Mod(std::string n, sol::function init) {
    name = n;
    description = "";
    onInit = init;
    onDash = std::nullopt;
    onFire = std::nullopt;
    onEnemyKill = std::nullopt;
    onEnemySpawn = std::nullopt;
}

Mod::Mod(std::string n) {
    name = n;
    description = "";
    onInit = std::nullopt;
    onDash = std::nullopt;
    onFire = std::nullopt;
    onEnemyKill = std::nullopt;
    onEnemySpawn = std::nullopt;
}

#define MODTHING(var, table, functionName, key) \
    {                                           \
        sol::function function = table[key];    \
        if(function.valid())                    \
            var.functionName = function;        \
        else                                    \
            var.functionName = std::nullopt;    \
    }

Mod Mod::fromTable(std::string n, sol::table table) {
    Mod x(n);

    MODTHING(x, table, onInit, "onInit");
    MODTHING(x, table, onDash, "onDash");
    MODTHING(x, table, onFire, "onFire");
    MODTHING(x, table, onEnemyKill, "onKill");
    MODTHING(x, table, onEnemySpawn, "onSpawn");

    x.description = (std::string)table["description"];
    return x;
}

#undef MODTHING

Mod::~Mod() {}

#include <optional>
#include <sol/forward.hpp>

#include "border.hpp"
#include "customEnemy.hpp"
#include "enemy.hpp"
#include "engine/entity.hpp"
#include "engine/world.hpp"
#include "mod.hpp"
#include "player.hpp"

ModManager::ModManager() : mods({}) {
    initLua();
}

ModManager::~ModManager() {}

void ModManager::onDash(unsigned playerId) {
    Player* player = (Player*)World::getEntity(playerId);
    if(player == nullptr) return;
    for(Mod& mod : mods)
        if(mod.onDash.has_value()) mod.onDash.value().call<void>(player);
}

void ModManager::onFire(unsigned playerId, unsigned bulletId) {
    Player* player = (Player*)World::getEntity(playerId);
    NodeBullet* bullet = (NodeBullet*)World::getEntity(bulletId);
    if(player == nullptr || bullet == nullptr) return;
    for(Mod& mod : mods)
        if(mod.onDash.has_value()) mod.onFire.value().call<void>(player, bullet);
}

void ModManager::onEnemyKill(unsigned playerId, unsigned enId) {
    Player* player = (Player*)World::getEntity(playerId);
    Enemy* en = (Enemy*)World::getEntity(enId);
    if(player == nullptr || en == nullptr) return;
    for(Mod& mod : mods)
        if(mod.onDash.has_value()) mod.onEnemyKill.value().call<void>(player, en);
}

void ModManager::onEnemySpawn(unsigned playerId, unsigned enId) {
    Player* player = (Player*)World::getEntity(playerId);
    Enemy* en = (Enemy*)World::getEntity(enId);
    if(player == nullptr || en == nullptr) return;
    for(Mod& mod : mods)
        if(mod.onDash.has_value()) mod.onEnemyKill.value().call<void>(player, en);
}

void ModManager::addMod(Mod mod, unsigned plrId) {
    Player* player = (Player*)World::getEntity(plrId);
    if(!mod.onInit.has_value()) {
        mods.push_back(mod);
        return;
    }
    auto t = mod.onInit.value()(player);
    // this is because a return code of `1` is a special case,
    // where you don't add the mod
    if(t.valid()) {
        if(t.get_type() == sol::type::number) {
            if((int)t == 1) {
                return;
            } else
                mods.push_back(mod);
        } else
            mods.push_back(mod);
    } else
        mods.push_back(mod);
}

int ModManager::loadMod(std::string mod, unsigned plrId) {
    fs::path path = mod::poolPath.string().append("/") + mod + ".lua";
    return loadModPath(path, plrId);
}

int ModManager::loadModPath(fs::path path, unsigned playerId) {
    if(!fs::exists(path)) return 1;
    std::optional<Mod> mod = fromPath(path);
    if(!mod.has_value()) {
        return 1;
    } else {
        addMod(mod.value(), playerId);
    }
    return 0;
}

void ModManager::loadMods(unsigned playerId) {
    for(auto& entry : fs::directory_iterator(mod::initPath)) loadModPath(entry.path(), playerId);
}

std::optional<Mod> ModManager::fromPath(fs::path x) {
    sol::function_result result = lua.script_file(x);
    return Mod::fromTable(x.filename().replace_extension("").string(), result);
}

std::optional<Mod> ModManager::fromName(std::string name) {
    std::string n = mod::poolPath;
    n.append(1, '/').append(name).append(".lua");
    Mod x = fromPath(n).value();
    return x;
}

std::string ModManager::pickRandomPoolMod() {
    const unsigned count = GetDirectoryFileCount(mod::poolPath.c_str());
    const unsigned i = (unsigned)((float)count * ((float)rand() / (float)RAND_MAX));
    unsigned index = 0;
    for(auto item : fs::directory_iterator(mod::poolPath))
        if(index++ >= i) return item.path().filename().replace_extension("");
    throw "Missing mod";
}

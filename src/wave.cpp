#include "wave.hpp"

#include "border.hpp"
#include "customEnemy.hpp"
#include "engine/world.hpp"
#include "store.hpp"

std::vector<std::function<void(void)>> WaveManager::waveBeginCallbacks = {};
std::vector<std::function<void(void)>> WaveManager::waveEndCallbacks = {};

const enum Tags tag = wave;

WaveManager::WaveManager() : Entity("wave manager") {
    waveI = 0;
    startStore();
}

WaveManager::~WaveManager() {}

void WaveManager::process(float delta) {
    std::vector<unsigned> t = World::getAllEntitiesWithTag(tag);
    if(inWave && t.size() == 0) startStore();
}

void WaveManager::startWave() {
    waveI++;
    inWave = true;
    // if(CustomEnemy::customEnemies.size() == 0) return;
    // for(int i = 0; i < waveI; i++) {
    // unsigned r = rand() % CustomEnemy::customEnemies.size();
    // std::unordered_map<std::string, CustomEnemy>::iterator t =
    // CustomEnemy::customEnemies.begin();
    // std::advance(t, r);
    Enemy* en = new Enemy(Border::getRandomPosInBorder());
    en->addTag(tag);
    World::addEntity(en);
    // }
}

void WaveManager::startStore() {
    Store* x = new Store({[this]() {
        this->startWave();
    }});
    World::addEntity(x);
    inWave = false;
}

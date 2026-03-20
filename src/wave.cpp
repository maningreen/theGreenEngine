#include "wave.hpp"

#include "border.hpp"
#include "customEnemy.hpp"
#include "engine/world.hpp"
#include "store.hpp"

const std::string WaveManager::waveBeginEvent = "waveBegin";
const std::string WaveManager::waveEndEvent = "waveEnd";

const enum Tags tag = wave;

void WaveManager::storeCloseCallback(Entity* t, void* _) {
    WaveManager* self = (WaveManager*)t;
    self->startWave();
}

WaveManager::WaveManager() : Entity("wave manager") {
    waveI = 0;
    startStore();
    World::listenEvent(Store::storeCloseEvent, &storeCloseCallback, getId());
}

WaveManager::~WaveManager() {}

void WaveManager::process(float delta) {
    std::vector<unsigned> t = World::getAllEntitiesWithTag(tag);
    if(inWave && t.size() == 0) startStore();
}

void WaveManager::startWave() {
    waveI++;
    inWave = true;
    World::callEvent(waveBeginEvent, nullptr);
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
    World::callEvent(waveEndEvent, nullptr);
    World::addEntity(new Store());
    inWave = false;
}

#include "wave.hpp"
#include "store.hpp"
#include "customEnemy.hpp"
#include "engine/core.h"

std::vector<std::function<void(void)>> WaveManager::waveBeginCallbacks = {};
std::vector<std::function<void(void)>> WaveManager::waveEndCallbacks = {};

std::string tag = "wave";

WaveManager::WaveManager() : Entity("wave manager") {
  waveI = 0;
  startStore();
}

WaveManager::~WaveManager() {}

void WaveManager::process(float delta) {
  std::vector<Entity*> t = Engine::getAllChildrenWithTag(this, tag);
  if(inWave && t.size() == 0)
    startStore();
}

void WaveManager::startWave() {
  waveI++;
  inWave = true;
  for(int i = 0; i < waveI; i++) {
    unsigned r = rand() % CustomEnemy::customEnemies.size();
    std::unordered_map<std::string, CustomEnemy>::iterator t = CustomEnemy::customEnemies.begin();
    std::advance(t, r);
    Enemy* en = new CustomEnemy(t->second);
    en->addTag(tag);
    addChild(en);
  }
}

void WaveManager::startStore() {
  Store* x = new Store;
  x->storeCloseCallbacks = {[this](){
    this->startWave();
  }};
  getRoot()->addChild(x);
  inWave = false;
}

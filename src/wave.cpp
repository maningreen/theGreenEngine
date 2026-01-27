#include "wave.hpp"
#include "store.hpp"
#include "customEnemy.hpp"

std::vector<std::function<void(void)>> WaveManager::waveBeginCallbacks = {};
std::vector<std::function<void(void)>> WaveManager::waveEndCallbacks = {};

WaveManager::WaveManager() : Entity("wave manager") {
  waveI = 0;
  startStore();
}

WaveManager::~WaveManager() {}

void WaveManager::process(float delta) {
}

void WaveManager::startWave() {
  waveI++;
  for(int i = 0; i < waveI; i++) {
    unsigned r = rand() % CustomEnemy::customEnemies.size();
    std::cout << CustomEnemy::customEnemies.size() << '\n';
    std::unordered_map<std::string, CustomEnemy>::iterator t = CustomEnemy::customEnemies.begin();
    std::advance(t, r);
    getRoot()->addChild(new CustomEnemy(t->second));
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

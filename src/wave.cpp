#include "wave.hpp"
#include "store.hpp"

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
  std::cout << "SDFL:KJSDF\n";
}

void WaveManager::startStore() {
  Store* x = new Store;
  x->storeCloseCallbacks = {[this](){
    this->startWave();
  }};
  getRoot()->addChild(x);
  inWave = false;
}

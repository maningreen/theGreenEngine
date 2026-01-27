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

}

void WaveManager::startStore() {
  getRoot()->addChild(new Store);
  inWave = false;
}

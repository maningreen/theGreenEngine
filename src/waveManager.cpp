#include "waveManager.hpp"
#include "engine/entity.hpp"

WaveManager::WaveManager() : Entity("waveManager"), waveCount(1) {
}

WaveManager::~WaveManager() {}

void WaveManager::Process(float delta) {
  //shrug we do nothin :p
  if(Children.size() != 0)
    return;
}

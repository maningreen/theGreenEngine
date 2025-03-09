#include "waveManager.hpp"
#include "enemy.hpp"
#include "border.hpp"
#include "spiral.hpp"
#include "dasher.hpp"
#include "engine/entity.hpp"

unsigned WaveManager::enemyTypeCount = 2;
float WaveManager::waveDelay = 5;

WaveManager::WaveManager() : Entity("waveManager"), waveCount(1), currentTime(0) {}

WaveManager::~WaveManager() {}

void WaveManager::Process(float delta) {
  if(Children.size() != 0)
    return;
  currentTime -= delta;
  if(currentTime <= 0)  // if we here we outa enemies and have waited long enough
    spawnWave();
}

void WaveManager::spawnWave() {
  for(int i = 0; i < waveCount; i++)
    spawnRandomEnemy(Border::getRandomPosInBorder());
  waveCount++;
  currentTime = waveDelay;
}

void WaveManager::spawnRandomEnemy(Vector2 p) {
  unsigned type = rand() % enemyTypeCount;
  Enemy* en;
  switch(type) {
    case 0: en = new Spiraler(p);
    break;
    case 1: en = new Dasher(p);
    break;
    default:;
  }
  addChild(en);
}

unsigned WaveManager::getWaveCount() {
  return waveCount;
}

unsigned WaveManager::getEnemyTypeCount() {
  return enemyTypeCount;
}


#include "waveManager.hpp"
#include "enemy.hpp"
#include "border.hpp"
#include "spiral.hpp"
#include "dasher.hpp"
#include "engine/entity.hpp"

unsigned WaveManager::enemyTypeCount = 2;

WaveManager::WaveManager() : Entity("waveManager"), waveCount(1) {
}

WaveManager::~WaveManager() {}

void WaveManager::Process(float delta) {
  if(Children.size() != 0)
    return;
  //if we're here that means we're outa enemies
  spawnWave();
}

void WaveManager::spawnWave() {
  for(int i = 0; i < waveCount; i++)
    spawnRandomEnemy(Border::getRandomPosInBorder());
  waveCount++;
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

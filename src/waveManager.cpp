#include "waveManager.hpp"
#include "enemy.hpp"
#include "border.hpp"
#include "spiral.hpp"
#include "dasher.hpp"
#include "particle.hpp"
#include "sniper.hpp"
#include "store.hpp"
#include "engine/entity.hpp"

unsigned WaveManager::enemyTypeCount = 3;
float WaveManager::upgradeOptions = 3;

unsigned WaveManager::upgradeCount = 4;
float* WaveManager::floatUpgrades[] = {
  &Player::dashSpeed,
  &Player::dashControl,
  &Particle::MaxLifetime
};

std::string WaveManager::upgradeNames[] = {
  "Dash Speed",
  "Dash Control",
  "Particle Lifetime"
};

WaveManager::WaveManager() : Entity("waveManager"), waveCount(1), inStore(false) {
  spawnWave();
}

WaveManager::~WaveManager() {}

void WaveManager::Process(float delta) {
  if(!inStore && Children.size() == 0) {
    unsigned uIn = rand() % upgradeCount;
    addChild(new StoreItem<float>(upgradeNames[uIn], floatUpgrades[uIn]));
    inStore = true;
  }
  else if(inStore && Children.size() == 0) {
    spawnWave();
    inStore = false;
  }
}

void WaveManager::spawnWave() {
  for(int i = 0; i < waveCount; i++)
    spawnRandomEnemy(Border::getRandomPosInBorder());
  waveCount++;
}

void WaveManager::spawnRandomEnemy(Vector2 p) {
  unsigned type = rand() % enemyTypeCount;
  Enemy* en;
  if(type == 0) en = new Dasher(p);
  else if(type == 1) en = new Spiraler(p);
  else if(type == 2) en = new Sniper(p);
  addChild(en);
}

unsigned WaveManager::getWaveCount() {
  return waveCount;
}

unsigned WaveManager::getEnemyTypeCount() {
  return enemyTypeCount;
}

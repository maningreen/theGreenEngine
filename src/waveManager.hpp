#ifndef waves_h
#define waves_h

#include "engine/entity.hpp"

class WaveManager : public Entity {
  private:
    unsigned waveCount;

    static unsigned enemyTypeCount;
    static float waveDelay;

    float currentTime;
  public:

    unsigned getWaveCount();
    unsigned getEnemyTypeCount();
    float getWaveDelay();
    float getTimeSinceLastWave();

    void Process(float delta) override;

    void spawnWave();

    void spawnRandomEnemy(Vector2 p);

    WaveManager();
    ~WaveManager();
};

#endif

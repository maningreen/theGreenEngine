#ifndef waves_h
#define waves_h

#include "engine/entity.hpp"

class WaveManager : public Entity {
  private:
    unsigned waveCount;

    static unsigned enemyTypeCount;
  public:

    unsigned getWaveCount();
    unsigned getEnemyTypeCount();

    void Process(float delta) override;

    void spawnWave();

    void spawnRandomEnemy(Vector2 p);

    WaveManager();
    ~WaveManager();
};

#endif

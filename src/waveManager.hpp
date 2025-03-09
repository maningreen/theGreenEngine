#ifndef waves_h
#define waves_h

#include "engine/entity.hpp"

class WaveManager : public Entity {
  private:
    unsigned waveCount;
  public:

    unsigned getWaveCount();

    void Process(float delta) override;

    WaveManager();
    ~WaveManager();
};

#endif

#ifndef WAVE_H
#define WAVE_H

#include <vector>

#include "engine/entity.hpp"

class WaveManager : public Entity {
  private:
    unsigned waveI;

    bool inWave;

    void startWave();
    void startStore();

    static void storeCloseCallback(Entity*, void*);

  public:
    static const std::string waveBeginEvent;
    static const std::string waveEndEvent;

    void process(float delta) override;

    WaveManager();
    ~WaveManager();
};

#endif

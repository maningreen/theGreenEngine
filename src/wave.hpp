#ifndef WAVE_H
#define WAVE_H

#include <functional>
#include <vector>

#include "engine/entity.hpp"

class WaveManager : public Entity {
  private:
    unsigned waveI;

    bool inWave;

    void startWave();
    void startStore();

  public:
    static std::vector<std::function<void(void)>> waveBeginCallbacks;
    static std::vector<std::function<void(void)>> waveEndCallbacks;

    void process(float delta) override;

    WaveManager();
    ~WaveManager();
};

#endif

#ifndef WAVE_H
#define WAVE_H

#include "engine/entity.hpp"
#include <vector>
#include <functional>

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

#ifndef CURSOR_H
#define CURSOR_H

#include "engine/entity.hpp"

class Cursor : public Entity2D {
private:
  float sigmaDelta;
public:
  void process(float delta) override;
  void render() override;

  static float thickness;
  static float length;
  static float offset;
  static float offsetTheta;
  static float centerRadius;
  static float waveSpeed;
  static float waveAmplitude;
  static unsigned markCount;

  Cursor();
  ~Cursor();
};

#endif

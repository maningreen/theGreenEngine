#ifndef dashNode_h
#define dashNode_h

#include "engine/entity.hpp"

class DashNode : public Entity2D {
private:
  float lifetime;

  float ease(float x);
public:
  void Render() override;
  void Process(float delta) override;

  static float radius;

  DashNode(Vector2 position);
};

#endif

#ifndef dashNode_h
#define dashNode_h

#include "engine/entity.hpp"
#include "laser.hpp"
#include <vector>

class DashNode : public Entity2D {
private:
  float lifetime;
  Laser* las;

  float radius;

  int getIndex();

  float ease(float x);
  static std::vector<DashNode*> nodes;
public:
  static float lifetimeAfterAttack;

  void Render() override;
  void Process(float delta) override;

  // returns the effective lifetime.
  float getMaxLifetime();

  static float defaultRadius;

  DashNode(Vector2 position);
  ~DashNode();
};

#endif

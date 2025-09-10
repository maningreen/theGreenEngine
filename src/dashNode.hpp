#ifndef dashNode_h
#define dashNode_h

#include "engine/entity.hpp"
#include "laser.hpp"
#include <vector>

class DashNode : public Entity2D {
private:
  float lifetime;
  float radius;

  Laser* las;

  float ease(float x);
  static std::vector<DashNode*> nodes;
public:
  void Render() override;
  void Process(float delta) override;

  int getIndex();

  DashNode(Vector2 position);
  ~DashNode();

  // returns the effective lifetime.
  float getMaxLifetime();

  static float defaultRadius;
  static float lifetimeAfterAttack;

  static std::vector<DashNode*> getNodes();
};

#endif

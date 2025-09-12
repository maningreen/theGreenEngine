#ifndef dashNode_h
#define dashNode_h

#include "engine/entity.hpp"
#include "laser.hpp"
#include <vector>

class DashNode : public Entity2D {
private:
  float lifetime;
  float radius;

  int index;

  Laser* las;

  float ease(float x);
  static std::vector<DashNode*> nodes;
public:
  void Render() override;
  void Process(float delta) override;

  int getIndex();

  DashNode* getNext();
  DashNode* getPrev();

  DashNode(Vector2 position);
  ~DashNode();

  // returns the effective lifetime.
  float getMaxLifetime();

  float getLasAngle();
  bool getBreakInLas();
  // returns 0 if invalid
  float getInternalAngle();

  static float defaultRadius;
  static float lifetimeAfterAttack;

  // returns the index of the first vertex that has a break 
  // returns -1 if there's no break, or if the polygon is invalid
  static int getBreakInPolygon();

  static std::vector<DashNode*> getNodes();
};

#endif

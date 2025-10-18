#ifndef attackNode_h
#define attackNode_h

#include "engine/entity.hpp"
#include "laser.hpp"
#include <vector>

class AttackNode : public Entity2D {
private:
  float lifetime;
  float radius;

  int index;

  Laser* las;

  float ease(float x);
  static std::vector<AttackNode*> nodes;
public:
  void Render() override;
  void Process(float delta) override;

  int getIndex();

  AttackNode* getNext();
  AttackNode* getPrev();

  AttackNode(Vector2 position);
  AttackNode(Vector2 position, bool addToList);
  ~AttackNode();

  // returns the effective lifetime.
  float getMaxLifetime();

  float getLasAngle();
  bool getBreakInLas();
  // returns 0 if invalid
  // RADIANS
  float getInternalAngle();

  // unwraps the current node relative to the previous node.
  // if they're already unwrapped applies no operations to `this`
  AttackNode unwrapRelative();

  static float defaultRadius;
  static float lifetimeAfterAttack;

  // returns the index of the first vertex that has a break 
  // returns -1 if there's no break, or if the polygon is invalid
  static int getBreakInPolygon();

  static std::vector<AttackNode*> getNodes();
};

#endif

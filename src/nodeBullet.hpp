#ifndef nodeBullet_h
#define nodeBullet_h

#include "engine/entity.hpp"

// A harmless projectile, that on death spawns an "attackNode"
class NodeBullet : public Entity2D {
public:
  // rotation of the "bullet"
  float theta;
  float lifetime;
  float targetLifetime;

  void Render() override;
  void Process(float delta) override;
  void Death() override;

  NodeBullet(Vector2 alpha, Vector2 beta, float r);
  NodeBullet(Vector2 alpha, float targetLifetime, float r);
  NodeBullet(Vector2 alpha, Vector2 beta);
  ~NodeBullet();

  static float speed;
  static float radius;
  static Color color;
};

#endif

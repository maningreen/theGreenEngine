#ifndef nodeBullet_h
#define nodeBullet_h

#include "engine/entity.hpp"
#include <string>

// A harmless projectile, that on death spawns an "attackNode"
struct NodeBullet : public Entity2D {
  // rotation of the "bullet"
  float theta;
  float lifetime;
  float targetLifetime;

  void render() override;
  void process(float delta) override;
  void death() override;

  NodeBullet(Vector2 alpha, Vector2 beta, float r);
  NodeBullet(Vector2 alpha, float targetLifetime, float r);
  NodeBullet(Vector2 alpha, Vector2 beta);
  ~NodeBullet();

  static float speed;
  static float radius;
  static Color color;

  static const std::string tag;
};

#endif

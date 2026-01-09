#ifndef healthPack_h
#define healthPack_h

#include "engine/entity.hpp"
#include "player.hpp"

class HealthPack : public Entity2D {
private:
  bool isAttracted;
public:
  float health;
  Vector2 velCart;
  float velPol;

  void process(float delta) override;
  void render() override;

  float getRadiusForHealth();

  HealthPack(Vector2 position, float h);
  HealthPack(Vector2 position, Vector2 inVel, float h);
  ~HealthPack();

  bool getIsAttracted();

  static const Color col;
  static const float decayRate;
  static const float playerPickupRadius;
};

#endif

#ifndef snip_h
#define snip_h

#include "enemy.hpp"
#include "laser.hpp"

//wooooo
class Sniper : public Enemy {
private:
  static float minDist;
  static float maxDist;
  static float rotationSpeed;
  static float speed;
  static float defaultHealth;
  static float maxDistFromTargetPos;
  static float widthGrowthRate;
  static float aimTime;
  static float shotRecoil;
  static float bulletSpeed;
  static float bulletDamage;
  static float radius;
  static Color laserColour;
  static Color defaultColour;

  float rotation;
  float rotationalVelocity;

  Vector2 targetPosition;

  Laser* las;

public:

  float getRotation() const;
  float getRotationalVelocity() const;
  Vector2 getTargetPosition() const;
  static float getMinDist();
  static float getMaxDist();
  static float getRotationSpeed();
  static float getSpeed();
  static float getDefaultHealth();
  static float getWidthGrowthRate();
  static float getAimTime();
  static float getShotRecoil();
  static float getBulletSpeed();
  static float getRadius();
  static float getBulletDamage();
  static Color getDefaultColour();
  static Color getLaserColour();

  Sniper(Vector2 position);
  ~Sniper();

  void manageStates(float delta) override;
  void dropHealth() override;

  enum states {
    positioning,
    aiming
  };
};

#endif

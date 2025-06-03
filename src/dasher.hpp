#ifndef dasher_h
#define dasher_h

#include "enemy.hpp"
#include "include.h"

class Dasher : public Enemy {
  private:
    Vector2 stateVector;

    static Color defaultCol;
    static float speed;
    static float radius;
    static float windupTime;
    static float windupSpeed;
    static float targetDist;
    static float maximumDist;
    static float recoveryTime;
    static float recoverSpeedThreshold;
    static float defaultHealth;
    static float dashTime;
    static float dashSpeed;
    static float damage;

    static float healthDropChance;

  public:

    //process
    void manageStates(float delta) override;
    // drop health
    void dropHealth() override;

    //getters
    static float getSpeed();
    static float getWindupTime();
    static float getWindupSpeed();
    static float getDefaultHealth();
    static float getDashTime();
    static float getDashSpeed();
    static float getRecoverTime();
    static float getRecoverSpeedThreshold();
    static float getDamage();
    static float getHealthDropChance();
    static Color getColour();

    //constructors
    Dasher(Vector2 pos);

    enum states {
      recovery,
      approaching,
      winding,
      dashing
    };
};

#endif

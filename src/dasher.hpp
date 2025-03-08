#ifndef dasher_h
#define dasher_h

#include "enemy.hpp"
#include "include.h"

class Dasher : public Enemy {
  private:
    float stateTime;
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

    void swapState(unsigned char s);
  public:

    //process
    void Process(float delta) override;

    //getters
    float getStateTime();
    float getSpeed();
    float getWindupTime();
    float getWindupSpeed();
    float getDefaultHealth();
    float getDashTime();
    float getDashSpeed();
    float getRecoverTime();
    float getRecoverSpeedThreshold();
    float getDamage();
    Color getColour();

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

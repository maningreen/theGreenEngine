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
    static float targetDist;
    static float maximumDist;
    static float recoveryTime;
    static float defaultHealth;
    static float dashTime;
    static float dashSpeed;

    void swapState();
  public:

    //process
    void Process(float delta) override;

    //getters
    float getStateTime();
    float getSpeed();
    float getWindupTime();
    float getDefaultHealth();
    float getDashTime();
    float getDashSpeed();
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

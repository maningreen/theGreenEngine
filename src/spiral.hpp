#ifndef spiral_h
#define spiral_h

#include "enemy.hpp"

class Spiraler : public Enemy {
  //check this shit out :sunglasses:
  //this is the first enemy made :D
  //(that isn't just the no ai one)
  private:
    Vector2 targetPosition;

    void swapToApproaching();
    void swapToSpinning();
  public:
    float spinTime;
    float shotTime;

    void manageStates(float delta) override; //yeah that's a good idea
    void dropHealth() override;

    Vector2 getTargetPosition();

    static Color defaultColour;
    static float DefaultRadius;
    static float SpinLength;
    static float SpinSpeed;
    static float spiralSpeed;
    static float startingHealth;
    static float maxTargetDistance;
    static float minTargetDistance;
    static float speed;
    static float bulletLifetime;
    static float bulletLifetimeGrowth;

    enum states {
      spinning,
      approaching
    };

    Spiraler(Vector2 position);
};

#endif

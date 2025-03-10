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
    static Color defaultColour;

    float rotation;
    float stateTime;

    Vector2 targetPosition;

    Laser* las;

  public:

    float getRotation() const;
    float getStateTime() const;
    Vector2 getTargetPosition() const;
    static float getMinDist();
    static float getMaxDist();
    static float getRotationSpeed();
    static float getSpeed();
    static Color getDefaultColour();

    Sniper(Vector2 position);
    ~Sniper();

    void manageStates(float delta) override;
    void Render() override;
  
    enum states {
      positioning,
      aiming
    };
};

#endif

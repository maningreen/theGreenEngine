#ifndef snip_h
#define snip_h

#include "enemy.hpp"

//wooooo
class Sniper : public Enemy {
  private:
    static float minDist;
    static float maxDist;
    static float rotationSpeed;
    static float speed;
    static Color defaultColour;

    float rotation;

  public:
    float getRotation() const;
    static float getMinDist();
    static float getMaxDist();
    static float getRotationSpeed();
    static Color getDefaultColour();

    Sniper(Vector2 position);
    ~Sniper();

    void Process(float delta) override;
  

    enum states {
      positioning,
      aiming
    };
};

#endif

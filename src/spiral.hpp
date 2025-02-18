#ifndef spiral_h
#define spiral_h

#include "enemy.hpp"

class Spiraler : public Enemy {
  //check this shit out :sunglasses:
  //this is the first enemy made :D
  //(that isn't just the no ai one)
  private:
    Vector2 targetPosition;
    float spinTime;
  public:

    void Process(float delta) override; //yeah that's a good idea
    void Render() override;

    Vector2 getTargetPosition();

    static Color Colour;
    static float SpinLength;
    static float SpinSpeed;
};

#endif

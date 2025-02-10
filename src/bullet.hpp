#ifndef bullet_h
#define bullet_h AA

#include "entity.hpp"
#include <raylib.h>

typedef Color Colour;

class Bullet : public Entity2D {
  public:
    Vector2 Velocity;
    float Angle;
    float Lifetime;

    virtual void Process(float delta);
    virtual void Render();

    Bullet(Vector2 position, float angle);
    ~Bullet();

    static float DefaultSpeed;
    static float MaxLifetime;
    static Vector2 bulletDimensions;
    static Colour DefaultColour;
};

#endif

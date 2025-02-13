#ifndef particle_h
#define particle_h

#include "entity.hpp"
#include <raylib.h>

class Particle : public Entity2D {
  void wrapPosition();
  float lifetime;

  public:
    float Rotation;
    float RotationalVelocity;
    float Length;
    Vector2 Velocity;
    Color Colour;

    void Process(float delta) override;
    void Render() override;

    float GetLifetime();

    //sets the rotation randomly and the rotational velocity between the given bounds:thumbsup:
    Particle(Vector2 position, Vector2 velocity, float maxRotVel, float minRotVel);
    //sets the rotation randomly and the rotational velocity randomly :)
    Particle(Vector2 position, Vector2 velocity);
    ~Particle();
};

#endif

#ifndef particle_h
#define particle_h

#include "engine/entity.hpp"
#include "include.h"

class Particle : public Entity2D {
  void wrapPosition();
  float lifetime;
  float maxLifetime;

  public:
    float Rotation;
    float RotationalVelocity;
    float Length;
    Vector2 Velocity;
    Color Colour;

    void Process(float delta) override;
    void Render() override;

    float GetLifetime();

    //sets the rotation randomly and the rotational velocity randomly :)
    Particle(Vector2 position, Vector2 velocity);
    ~Particle();

    static float MinLifetime;
    static float MaxLifetime;
};

#endif

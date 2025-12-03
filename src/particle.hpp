#ifndef particle_h
#define particle_h

#include "engine/entity.hpp"
#include "include.h"

class Particle : public Entity2D {
  float lifetime;
  float maxLifetime;

  public:
    float rotation;
    float rotationalVelocity;
    float length;
    Vector2 velocity;
    Color colour;

    void process(float delta) override;
    void render() override;

    float GetLifetime();

    //sets the rotation randomly and the rotational velocity randomly :)
    Particle(Vector2 position, Vector2 velocity);
    ~Particle();

    static float MinLifetime;
    static float MaxLifetime;
};

#endif

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
    bool hasGravity;

    Vector2 velocity;
    Color colour;

    void process(float delta) override;
    void render() override;

    float getLifetime();

    static float gravity;

    // sets the rotation randomly and the rotational velocity randomly :)
    Particle(Vector2 position, Vector2 velocity);
    // sets the rotation randomly and the rotational velocity randomly, with/without gravity
    Particle(Vector2 position, Vector2 velocity, bool gravity);
    // sets the rotation randomly and the rotational velocity randomly, specify length, with/without
    // gravity
    Particle(Vector2 position, Vector2 velocity, float length, bool gravity);
    ~Particle();

    static float MinLifetime;
    static float MaxLifetime;
};

#endif

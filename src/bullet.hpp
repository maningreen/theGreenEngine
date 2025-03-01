#include <vector>
#ifndef bullet_h
#define bullet_h AA

#include "engine/entity.hpp"
#include "enemy.hpp"
#include "include.h"

typedef Color Colour;

bool CheckCollisionCircleRecEx(Vector2 center, float radius, Vector2 rectPos, Vector2 dimensions, float angle);

class Bullet : public Entity2D {
  public:
    Vector2 Velocity;
    float Angle;
    float Lifetime;
    Colour col;

    void Process(float delta) override;
    void Render() override;
    void Init() override;
    void wrapPosition();

    virtual bool ManageCollision();

    Bullet(Vector2 position, float angle);
    ~Bullet();

    static float DefaultSpeed;
    static float MaxLifetime;
    static Vector2 bulletDimensions;
    static Colour DefaultColour;

    static float Damage;
};

#endif

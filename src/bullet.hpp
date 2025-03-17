#include <vector>
#ifndef bullet_h
#define bullet_h AA

#include "engine/entity.hpp"
#include "include.h"

bool CheckCollisionCircleRecEx(Vector2 center, float radius, Vector2 rectPos, Vector2 dimensions, float angle);

class Bullet : public Entity2D {
  public:
    Vector2 Velocity;
    float Angle;
    float Lifetime;
    Color col;
    bool shouldWrap;

    void Process(float delta) override;
    void Render() override;
    void Init() override;

    virtual bool ManageCollision();

    Bullet(Vector2 position, float angle);
    Bullet(Vector2 position, float angle, bool shouldWrap);
    Bullet(Vector2 position, float angle, float maxLifetime);
    Bullet(Vector2 position, float angle, bool shouldWrap, float maxLifetime);
    Bullet(Vector2 position, float angle, bool shouldWrap, float maxLifetime, float speed);
    ~Bullet();

    static float DefaultSpeed;
    static float DefaultMaxLifetime;
    static Vector2 bulletDimensions;
    static Color DefaultColour;

    static float Damage;
};

#endif

#include <vector>
#ifndef bullet_h
#define bullet_h AA

#include "entity.hpp"
#include "enemy.hpp"
#include <raylib.h>

typedef Color Colour;

class Bullet : public Entity2D {
  public:
    Vector2 Velocity;
    float Angle;
    float Lifetime;

    void Process(float delta) override;
    void Render() override;
    void Init() override;


    void wrapPosition();

    Bullet(Vector2 position, float angle);
    ~Bullet();

    static float DefaultSpeed;
    static float MaxLifetime;
    static Vector2 bulletDimensions;
    static Colour DefaultColour;

    static std::vector<Enemy*> Enemies;

};

#endif

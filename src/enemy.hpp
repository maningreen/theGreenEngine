#ifndef enemy_h
#define enemy_h

#include "entity.hpp"
#include "player.hpp"
#include "bars.hpp"
#include "include.h"

class Enemy : public Entity2D {
  public:
    Vector2 Velocity;
    Vector2 TargetPos;

    float Radius;

    Player* plr;

    float health;
    Bar* healthBar;

    void Process(float delta) override;
    void Render() override;
    void Init() override;

    virtual Vector2 getNextVelocity(float delta);

    Vector2 getShortestVectorToPlayer();

    Vector2 GetNextTargetPosition();
    void WrapPosition();

    void applyDamage(float damage);

    bool isAlive();

    void setPlayer();

    void manageBar();
    void manageHeath();

    Enemy(Vector2 position);
    ~Enemy();

    static float DefaultRadius;
    static float Speed;
    static Color Colour;

    static float MaxHealth;
};

#endif

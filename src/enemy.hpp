#ifndef enemy_h
#define enemy_h

#include "entity.hpp"
#include "player.hpp"
#include "bars.hpp"
#include <raylib.h>

class Enemy : public Entity2D {
  private:
    Player* plr;

    float health;

    Bar* healthBar;

    void manageBar();

    void manageHeath();
  public:
    Vector2 Velocity;
    Vector2 TargetPos;

    void Process(float delta) override;
    void Render() override;
    void Init() override;

    virtual Vector2 getNextVelocity(float delta);

    Vector2 getShortestVectorToPlayer();

    Vector2 GetNextTargetPosition();
    void WrapPosition();

    float getHealth();

    void applyDamage(float damage);

    bool isAlive();

    Enemy(Player* pl, Vector2 position);
    ~Enemy();

    static float Radius;
    static float Speed;
    static Color Colour;

    static float MaxHealth;
};

#endif

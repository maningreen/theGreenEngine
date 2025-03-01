#ifndef enemy_h
#define enemy_h

#include "entity.hpp"
#include "player.hpp"
#include "healthManager.hpp"
#include "barManager.hpp"
#include "include.h"

class Enemy : public Entity2D {
  public:
    Vector2 Velocity;
    Vector2 TargetPos;

    float Radius;

    Player* plr;

    HealthManager* healthManager;

    void Process(float delta) override;
    void Render() override;
    void Init() override;

    virtual Vector2 getNextVelocity(float delta);

    Vector2 getShortestVectorToPlayer();

    Vector2 GetNextTargetPosition();
    void WrapPosition();

    bool isAlive();

    void setPlayer();

    void manageHealthBar(float radius);

    Enemy(Vector2 position);
    ~Enemy();

    static float DefaultRadius;
    static float Speed;
    static Color Colour;
};

#endif

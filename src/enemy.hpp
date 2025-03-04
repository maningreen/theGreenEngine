#ifndef enemy_h
#define enemy_h

#include "engine/entity.hpp"
#include "player.hpp"
#include "healthManager.hpp"
#include "barManager.hpp"
#include "include.h"

class Enemy : public Entity2D {
  public:
    Vector2 Velocity;
    Vector2 TargetPos;

    float Radius;

    HealthManager* healthManager;

    void Process(float delta) override;
    void Render() override;
    void Init() override;

    Vector2 getShortestVectorToPlayer();

    void WrapPosition();

    void manageHealthBar(float radius);

    Vector2 getClosestPointToPlayerWithDistance(float distance);

    Enemy(Vector2 position);
    ~Enemy();

    static Player* plr;
    static void setPlayer();

    static float DefaultRadius;
    static float Speed;
    static float friction;
    static Color Colour;
};

#endif

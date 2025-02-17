#ifndef enemy_h
#define enemy_h

#include "entity.hpp"
#include "player.hpp"
#include <raylib.h>

class Enemy : public Entity2D {
  private:
    Player* plr;
  public:
    Vector2 Velocity;
    Vector2 TargetPos;

    void Process(float delta) override;
    void Render() override;
    void Init() override;

    Vector2 getShortestVectorToPlayer();

    Vector2 GetNextTargetPosition();
    void WrapPosition();

    Enemy(Player* pl, Vector2 position);
    ~Enemy();

    static float Radius;
    static float Speed;
    static Color Colour;
};

#endif

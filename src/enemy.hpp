#ifndef enemy_h
#define enemy_h

#include "engine/entity.hpp"
#include "player.hpp"
#include "healthManager.hpp"
#include "barManager.hpp"
#include "include.h"

class Enemy : public Entity2D {
  private:
    unsigned char state;

    float stateTime;
    static Player* plr;

    HealthManager* healthManager;
    void manageHealthBar(float radius);

  public:
    Vector2 Velocity;
    Vector2 TargetPos;

    float Radius;

    Vector2 getShortestVectorToPlayer() const;
    float getAngleToPlayer() const;

    HealthManager* getHealthManager() const;

    void setState(unsigned char s);
    unsigned char getState() const;

    float getStateTime() const;
    void resetStateTime();

    Vector2 getClosestPointToPlayerWithDistance(float distance) const;

    void fireBullet(float angle, float lifetime, Color col) const;
    void fireBullet(float angle, float lifetime, float speed, Color col) const;

    void Process(float delta) override;
    void Render() override;
    void Init() override;

    virtual void manageStates(float delta) {};

    Enemy(Vector2 position);
    ~Enemy();

    static void setPlayer();
    static void setPlayer(Player* address);
    static Player* getPlayer();

    static float DefaultRadius;
    static float Speed;
    static float friction;
    static Color Colour;
};

#endif

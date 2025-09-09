#ifndef enemy_h
#define enemy_h

#include "engine/entity.hpp"
#include "healthManager.hpp"
#include "include.h"
#include "enemyBullet.hpp"

class Enemy : public Entity2D {
  private:
    unsigned char state;

    float stateTime;
    static Entity2D* plr;

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

    EnemyBullet* fireBullet(float angle, float lifetime, Color col) const;
    EnemyBullet* fireBullet(float angle, float lifetime, float speed, Color col) const;
    EnemyBullet* fireBullet(float angle, float lifetime, float speed, float dmg, Color col) const;

    void Process(float delta) override;
    void Render() override;
    void Death() override;
    void Init() override;

    virtual void manageStates(float delta) {};
    virtual void dropHealth() {};

    void dropHealthPack();
    void dropHealthPack(float hp, Entity* root);

    Enemy(Vector2 position);
    ~Enemy();

    static void setPlayer();
    static void setPlayer(Entity2D* address);
    static Entity2D* getPlayer();

    static float DefaultRadius;
    static float Speed;
    static float friction;

    static float droppedHealthHP;
    Color Colour;
};

#endif

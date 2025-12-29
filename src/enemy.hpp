#ifndef enemy_h
#define enemy_h

#include "engine/entity.hpp"
#include "healthManager.hpp"
#include "enemyBullet.hpp"
#include <functional>
#include <string>

class Enemy : public Entity2D {
  private:
    unsigned char state;

    float stateTime;

    HealthManager* healthManager;
    void manageHealthBar(float radius);

    static std::vector<std::function<void(Enemy*)>> onSpawnHooks;
    static std::vector<std::function<void(Enemy*)>> onDeathHooks;

  public:
    Vector2 velocity;
    Vector2 targetPos;

    float radius;

    Vector2 getShortestVectorToPlayer() const;
    float getAngleToPlayer() const;

    HealthManager* getHealthManager();

    void setState(unsigned char s);
    unsigned char getState() const;

    float getStateTime() const;
    void resetStateTime();

    Vector2 getClosestPointToPlayerWithDistance(float distance) const;

    EnemyBullet* fireBullet(float angle, float lifetime, Color col) const;
    EnemyBullet* fireBullet(float angle, float lifetime, float speed, Color col) const;
    EnemyBullet* fireBullet(float angle, float lifetime, float speed, float dmg, Color col) const;

    void process(float delta) override;
    void render() override;
    void death() override;
    void init() override;

    virtual void onDeath() {};
    virtual void onSpawn() {};

    virtual void manageStates(float delta) {};
    virtual void dropHealth() {};

    void dropHealthPack();
    void dropHealthPack(float hp);

    Enemy(Vector2 position);
    ~Enemy();

    static float DefaultRadius;
    static float Speed;

    static void addSpawnHook(std::function<void(Enemy*)>);
    static void addDeathHook(std::function<void(Enemy*)>);

    static const std::string tag;

    static float droppedHealthHP;
    Color colour;
};

#endif

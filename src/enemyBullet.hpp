#ifndef enemyBul_h
#define enemyBul_h

#include "bullet.hpp"

class EnemyBullet : public Bullet {
  public:
    bool ManageCollision() override;
    float damage;

    EnemyBullet(Vector2 position, float angle, Color col);
    EnemyBullet(Vector2 position, float angle, Color col, bool shouldWrap);
    EnemyBullet(Vector2 position, float angle, Color col, bool shouldWrap, float lifetime);
    EnemyBullet(Vector2 position, float angle, Color col, bool shouldWrap, float lifetime, float speed);
    EnemyBullet(Vector2 position, float angle, Color col, bool shouldWrap, float lifetime, float speed, float damage);
    ~EnemyBullet();
};

#endif

#ifndef enemyBul_h
#define enemyBul_h

#include "bullet.hpp"
#include "enemy.hpp"
#include "player.hpp"

class EnemyBullet : public Bullet {
  public:
    bool ManageCollision() override;

    EnemyBullet(Vector2 position, float angle, Color col);
    EnemyBullet(Vector2 position, float angle, Color col, bool shouldWrap);
    ~EnemyBullet();
};

#endif

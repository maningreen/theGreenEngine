#ifndef enemyBul_h
#define enemyBul_h

#include "bullet.hpp"
#include "enemy.hpp"
#include "player.hpp"

class EnemyBullet : public Bullet {
  public:
    static Player* Plr;

    bool ManageCollision() override;

    Color Colour;

    EnemyBullet(Vector2 position, float angle, Color col);
    ~EnemyBullet();
};

#endif

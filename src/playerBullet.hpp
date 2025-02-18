#ifndef playerBull_h
#define playerBull_h

#include "bullet.hpp"
#include "enemy.hpp"
#include <raylib.h>
#include <vector>

class PlayerBullet : public Bullet {
  public:
    static std::vector<Enemy*> enemies;

    bool ManageCollision() override;

    PlayerBullet(Vector2 position, float angle);
    ~PlayerBullet();
};

#endif

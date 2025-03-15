#include "enemyBullet.hpp"
#include "bullet.hpp"
#include "enemy.hpp"
#include "player.hpp"
#include "include.h"

EnemyBullet::EnemyBullet(Vector2 position, float angle, Color c) : Bullet(position, -angle)  {
  //NOTHING YEAHHHHHHHHHHHHHH WOOOOOOOOOOOOOOOOOO
  //wait
  //we have to set the colour D:
  col = c;
}

EnemyBullet::EnemyBullet(Vector2 position, float angle, Color c, bool w) : Bullet(position, -angle, w)  {
  //NOTHING YEAHHHHHHHHHHHHHH WOOOOOOOOOOOOOOOOOO
  //wait
  //we have to set the colour D:
  col = c;
}

EnemyBullet::EnemyBullet(Vector2 position, float a, Color c, bool w, float l) : Bullet(position, -a, w, l) {
  col = c;
}

EnemyBullet::EnemyBullet(Vector2 position, float a, Color c, bool w, float l, float speed) : Bullet(position, -a, w, l, speed) {
  col = c;
}

EnemyBullet::~EnemyBullet() {}

bool EnemyBullet::ManageCollision() {
  //now we do some shmath
  if(Enemy::getPlayer() == nullptr)
    return false;

  bool hit = CheckCollisionCircleRecEx(Enemy::getPlayer()->Position, Player::hitboxRadius, Position, bulletDimensions, Angle); //(we've done it all before)
  if(hit)
    Enemy::getPlayer()->getHealthManager()->applyDamage(1);
  return hit;
}

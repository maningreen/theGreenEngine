#include "enemyBullet.hpp"
#include "bullet.hpp"
#include "enemy.hpp"
#include "player.hpp"
#include "include.h"

EnemyBullet::EnemyBullet(Vector2 position, float angle, Color c) : Bullet(position, -angle), damage(1)  {
  //NOTHING YEAHHHHHHHHHHHHHH WOOOOOOOOOOOOOOOOOO
  //wait
  //we have to set the colour D:
  col = c;
}

EnemyBullet::EnemyBullet(Vector2 position, float angle, Color c, bool w) : Bullet(position, -angle, w), damage(1) {
  //NOTHING YEAHHHHHHHHHHHHHH WOOOOOOOOOOOOOOOOOO
  //wait
  //we have to set the colour D:
  col = c;
}

EnemyBullet::EnemyBullet(Vector2 position, float a, Color c, bool w, float l) : Bullet(position, -a, w, l), damage(1) {
  col = c;
}

EnemyBullet::EnemyBullet(Vector2 position, float a, Color c, bool w, float l, float speed) : Bullet(position, -a, w, l, speed), damage(1) {
  col = c;
}

EnemyBullet::EnemyBullet(Vector2 position, float a, Color c, bool w, float l, float speed, float damage) : Bullet(position, -a, w, l, speed), damage(damage) {
  col = c;
}

EnemyBullet::~EnemyBullet() {}

bool EnemyBullet::ManageCollision() {
  //now we do some shmath
  if(Enemy::getPlayer() == nullptr)
    return false;

  bool hit = CheckCollisionCircleRecEx(Enemy::getPlayer()->position, Player::hitboxRadius, position, bulletDimensions, Angle); //(we've done it all before)
  if(hit)
    ((Player*)Enemy::getPlayer())->getHealthManager()->applyDamage(damage);
  return hit;
}

#include "enemyBullet.hpp"
#include "bullet.hpp"
#include "player.hpp"
#include "include.h"

Player* EnemyBullet::Plr = nullptr;

EnemyBullet::EnemyBullet(Vector2 position, float angle, Color c) : Bullet(position, angle)  {
  //NOTHING YEAHHHHHHHHHHHHHH WOOOOOOOOOOOOOOOOOO
  //wait
  //we have to set the colour D:
  col = c;
}

EnemyBullet::EnemyBullet(Vector2 position, float angle, Color c, bool w) : Bullet(position, angle, w)  {
  //NOTHING YEAHHHHHHHHHHHHHH WOOOOOOOOOOOOOOOOOO
  //wait
  //we have to set the colour D:
  col = c;
}

EnemyBullet::~EnemyBullet() {}

bool EnemyBullet::ManageCollision() {
  //now we do some shmath
  bool hit = CheckCollisionCircleRecEx(Plr->Position, Player::hitboxRadius, Position, bulletDimensions, Angle); //(we've done it all before)

  if(hit)
    Plr->getHealthManager()->applyDamage(1);
  return hit;
}

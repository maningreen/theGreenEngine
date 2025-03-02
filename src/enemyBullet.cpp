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
  return 0; // :)
}

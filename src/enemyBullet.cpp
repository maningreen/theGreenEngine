#include "enemyBullet.hpp"
#include "bullet.hpp"
#include "player.hpp"
#include "include.h"

Player* EnemyBullet::Plr = nullptr;

EnemyBullet::EnemyBullet(Vector2 position, float angle, Color col) : Bullet(position, angle), Colour(col) {
  //NOTHING YEAHHHHHHHHHHHHHH WOOOOOOOOOOOOOOOOOO
  //wait
  //we have to set the colour D: (we already did that)
}

EnemyBullet::~EnemyBullet() {}

bool EnemyBullet::ManageCollision() {
  return 0; // :)
}

#include "playerBullet.hpp"
#include "enemy.hpp"
#include <cmath>
#include <vector>
#include <raylib.h>
#include <raymath.h>

std::vector<Enemy*> PlayerBullet::enemies;

bool PlayerBullet::ManageCollision() {
  //here we have to fucking kill ourselves i love this i love this i love this
  for(Enemy* en : enemies)
    if(CheckCollisionCircleRecEx(en->Position, en->Radius, Position, bulletDimensions, Angle * M_PI / 180.0f)) {
      en->applyDamage(Damage);
      return true;
    }
  return false;
}

PlayerBullet::PlayerBullet(Vector2 position, float angle) : Bullet(position, angle) {
  //we do nothing here lol
}

PlayerBullet::~PlayerBullet() {
  //shrug
}

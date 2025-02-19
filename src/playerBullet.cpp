#include "playerBullet.hpp"
#include "enemy.hpp"
#include <cmath>
#include <vector>
#include <raylib.h>
#include <raymath.h>

std::vector<Enemy*> PlayerBullet::enemies;

bool CheckCollisionCircleRecEx(Vector2 center, float radius, Vector2 rectPos, Vector2 dimensions, float angle) {
  //...
  //oh god here we go
  //step one figure out the center
  Vector2 diff = Vector2Subtract(center, rectPos);
  Vector2 angleVec = (Vector2){cosf(-angle), sinf(-angle)};
  //then we transform the circle to local space
  Vector2 localPos = (Vector2){diff.x * angleVec.x - diff.y * angleVec.y, diff.x * angleVec.y + diff.y * angleVec.x};
  Vector2 dems = Vector2Scale(dimensions, .5f);
  Vector2 finalPos = (Vector2){
    fmaxf(-dems.x, fminf(localPos.x, dems.x)),
      fmaxf(-dems.y, fminf(localPos.y, dems.y))
  };
  return Vector2Distance(finalPos, localPos) <= radius;
}

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

#include "bullet.hpp"
#include "enemy.hpp"
#include "entity.hpp"
#include "border.hpp"
#include "core.h"
#include <cmath>
#include <cstdlib>
#include <raylib.h>
#include <raymath.h>
#include <iostream>
#include <vector>

#define max(a, b) (b < a ? a : b)
#define stepCount 20

float Bullet::DefaultSpeed = 3040;
float Bullet::MaxLifetime = 1;
Vector2 Bullet::bulletDimensions = (Vector2){30, 15};
Colour Bullet::DefaultColour = YELLOW;

std::vector<Enemy*> Bullet::Enemies;

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

Bullet::Bullet(Vector2 position, float angle) : Entity2D("Bullet", position), Angle(angle) {
  Velocity = (Vector2){cosf(angle) * DefaultSpeed, -sinf(angle) * DefaultSpeed};
  Lifetime = 0;
}

void Bullet::Init() {
}

Bullet::~Bullet() {}

void Bullet::Process(float delta) {
  //check if colliding with enemy
  for(int i = 0; i < stepCount; i++) {
    if(valid) {
      for(Enemy* en : Enemies) {
        if(!en->valid)
          continue;
        if(CheckCollisionCircleRecEx(en->Position, en->Radius, Position, bulletDimensions, Angle * M_PI / 180.0f)) {
          en->valid = false;
          valid = false;
          break;
        }
      }
    }
    else
      break;
    Position = Vector2Add(Position, Vector2Scale(Velocity, delta / stepCount));
    wrapPosition();
  }

  Lifetime += delta;
  valid = valid && Lifetime < MaxLifetime;
}

void Bullet::wrapPosition() {
  if(Position.x >= Border::Length)
    Position.x -= Border::Length * 2;
  else if(Position.x <= -Border::Length)
    Position.x += Border::Length * 2;
  if(Position.y >= Border::Length)
    Position.y -= Border::Length * 2;
  else if(Position.y <= -Border::Length)
    Position.y += Border::Length * 2;
}

void Bullet::Render() {
  //TIME FOR SOME FUCKY FUN MATH :DDDDDDD
  //so here is our wanted result
  //we want a nice rectangle (dimensions BulletDimensions)
  //with center Position
  //IF ONLY THERE WAS A FUNCTION FOR THIS
  DrawRectanglePro((Rectangle){Position.x, Position.y, bulletDimensions.x, bulletDimensions.y}, (Vector2){bulletDimensions.x / 2.0f, bulletDimensions.y / 2.0f}, -Angle * 180 / M_PI, DefaultColour);
}

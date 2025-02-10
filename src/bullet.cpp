#include "bullet.hpp"
#include "entity.hpp"
#include <raylib.h>
#include <raymath.h>

float Bullet::DefaultSpeed = 3040;
float Bullet::MaxLifetime = 1;
Vector2 Bullet::bulletDimensions = (Vector2){30, 15};
Colour Bullet::DefaultColour = YELLOW;

Bullet::Bullet(Vector2 position, float angle) : Entity2D("Bullet", position), Angle(angle) {
  Velocity = (Vector2){cosf(angle) * DefaultSpeed, -sinf(angle) * DefaultSpeed};
  Lifetime = 0;
}

Bullet::~Bullet() {}

void Bullet::Process(float delta) {
  Position = Vector2Add(Position, Vector2Scale(Velocity, delta));
  Lifetime += delta;
  valid = valid && Lifetime < MaxLifetime;
}

void Bullet::Render() {
  //TIME FOR SOME FUCKY FUN MATH :DDDDDDD
  //so here is our wanted result
  //we want a nice rectangle (dimensions BulletDimensions)
  //with center Position
  //IF ONLY THERE WAS A FUNCTION FOR THIS
  DrawRectanglePro((Rectangle){Position.x, Position.y, bulletDimensions.x, bulletDimensions.y}, (Vector2){bulletDimensions.x / 2.0f, bulletDimensions.y / 2.0f}, -Angle * 180 / M_PI, DefaultColour);
}

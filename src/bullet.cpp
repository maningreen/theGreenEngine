#include "bullet.hpp"
#include "engine/entity.hpp"
#include "border.hpp"
#include <cmath>
#include <cstdlib>

#define stepCount 20

float Bullet::DefaultSpeed = 3040;
float Bullet::DefaultMaxLifetime = 1;
Vector2 Bullet::bulletDimensions = (Vector2){30, 15};
Color Bullet::DefaultColour = YELLOW;

float Bullet::Damage = 1;

Bullet::Bullet(Vector2 position, float angle) : Entity2D("Bullet", position), Angle(angle) {
  velocity = (Vector2){cosf(angle) * DefaultSpeed, -sinf(angle) * DefaultSpeed};
  col = DefaultColour;
  lifetime = DefaultMaxLifetime;
  shouldWrap = true;
}

Bullet::Bullet(Vector2 position, float angle, bool w) : Entity2D("Bullet", position), Angle(angle), shouldWrap(w) {
  velocity = (Vector2){cosf(angle) * DefaultSpeed, -sinf(angle) * DefaultSpeed};
  col = DefaultColour;
  lifetime = DefaultMaxLifetime;
}

Bullet::Bullet(Vector2 position, float angle, float maxLifetime) : Entity2D("bullet", position), Angle(angle), lifetime(maxLifetime) {
  velocity = (Vector2){cosf(angle) * DefaultSpeed, -sinf(angle) * DefaultSpeed};
  col = DefaultColour;
}

Bullet::Bullet(Vector2 position, float angle, bool shouldWrap, float maxLifetime) : 
  Entity2D("bullet", position), 
  Angle(angle), 
  shouldWrap(shouldWrap), 
  lifetime(maxLifetime) {
  velocity = (Vector2){cosf(angle) * DefaultSpeed, -sinf(angle) * DefaultSpeed};
  col = DefaultColour;
}

Bullet::Bullet(Vector2 position, float angle, bool shouldWrap, float maxLifetime, float speed) : 
  Entity2D("bullet", position), 
  Angle(angle), 
  shouldWrap(shouldWrap), 
  lifetime(maxLifetime) {
  velocity = (Vector2){cosf(angle) * speed, -sinf(angle) * speed};
  col = DefaultColour;
}

void Bullet::init() {}

Bullet::~Bullet() {}

void Bullet::process(float delta) {
  //check if colliding with enemy
  for(int i = 0; i < stepCount; i++) {
    if(ManageCollision()) {
      killDefered();
      return;       //make it smart as shit
    }
    position = Vector2Add(position, Vector2Scale(velocity, delta / stepCount));
    if(shouldWrap)
      Border::wrapEntity(this);
  }
  lifetime -= delta;
  if(lifetime < 0)
    killDefered();
}

bool Bullet::ManageCollision() {
  return false;
}

void Bullet::render() {
  //TIME FOR SOME FUCKY FUN MATH :DDDDDDD
  //so here is our wanted result
  //we want a nice rectangle (dimensions BulletDimensions)
  //with center Position
  //IF ONLY THERE WAS A FUNCTION FOR THIS
  //he said with a gleam in his eye
  DrawRectanglePro((Rectangle){position.x, position.y, bulletDimensions.x, bulletDimensions.y}, (Vector2){bulletDimensions.x / 2.0f, bulletDimensions.y / 2.0f}, -Angle * 180 / M_PI, col);
}

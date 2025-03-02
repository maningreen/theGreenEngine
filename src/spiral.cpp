#include "spiral.hpp"
#include "enemy.hpp"
#include "include.h"
#include "enemyBullet.hpp"
#include <cmath>
#include <iostream>

Color Spiraler::Colour = BLUE;
float Spiraler::SpinLength = 10;
float Spiraler::SpinSpeed = 10;
float Spiraler::DefaultRadius = 50;
float Spiraler::spiralSpeed = M_PI / 3.0f;
float Spiraler::startingHealth = 3;

#define bulletsPerShot 3

Spiraler::Spiraler() : Enemy(Vector2Zero()), isSpinning(true) {
  Radius = DefaultRadius;
  shotTime = 1.0f / 10.0f;
  healthManager->maxHealth = 2;
}

void Spiraler::Render() {
  DrawCircleV(Position, Radius, Colour);
}

void Spiraler::Process(float delta) {
  manageHealthBar(Radius);
  spinTime += delta; 
  if(fmodf(spinTime, shotTime) < 1.0f / 60.0f)
    for(int i = 0; i < bulletsPerShot; i++)
    getRoot()->addChild(new EnemyBullet(Position, spinTime * spiralSpeed + 2 * M_PI * i / bulletsPerShot, Colour, false));
}

#include "spiral.hpp"
#include "bars.hpp"
#include "enemy.hpp"
#include "engine/core.h"
#include "healthManager.hpp"
#include "include.h"
#include "enemyBullet.hpp"
#include <cmath>
#include <iostream>

Color Spiraler::Colour = BLUE;
float Spiraler::SpinLength = 5;
float Spiraler::SpinSpeed = 10;
float Spiraler::DefaultRadius = 50;
float Spiraler::spiralSpeed = M_PI / 3.0f;
float Spiraler::startingHealth = 3;

#define bulletsPerShot 3

#define barDimensions (Vector2){Radius * 2, 10}

Spiraler::Spiraler() : Enemy(Vector2Zero()), state(spinning) {
  Radius = DefaultRadius;
healthManager->setMaxHealth(startingHealth);
  shotTime = 1.0f / 10.0f;
}

void Spiraler::Render() {
  DrawCircleV(Position, Radius, Colour);
}

void Spiraler::Process(float delta) {
  manageHealthBar(Radius);
  spinTime += delta; 
  if(state == spinning) {
    if(fmodf(spinTime, shotTime) < 1.0f / 60.0f)
      for(int i = 0; i < bulletsPerShot; i++)
    getRoot()->addChild(new EnemyBullet(Position, spinTime * spiralSpeed + (2 * M_PI * i / bulletsPerShot), Colour, false));
    if(spinTime > SpinLength) {
      state = approaching;
      spinTime = 0;
    }
  } else if(spinTime > SpinLength) {
      state = spinning;
      spinTime = 0;
  }
}

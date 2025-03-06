#include "spiral.hpp"
#include "bars.hpp"
#include "enemy.hpp"
#include "engine/core.h"
#include "healthManager.hpp"
#include "include.h"
#include "enemyBullet.hpp"
#include "border.hpp"
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <vector>

#define min(a, b) (b < a ? b : a)

Color Spiraler::Colour = BLUE;
float Spiraler::SpinLength = 5;
float Spiraler::SpinSpeed = 10;
float Spiraler::DefaultRadius = 50;
float Spiraler::spiralSpeed = M_PI / 3.0f;
float Spiraler::startingHealth = 3;
float Spiraler::maxTargetDistance = 770;
float Spiraler::minTargetDistance = 600;
float Spiraler::speed = 2100;
float Spiraler::bulletLifetime = .5;

#define bulletsPerShot 3

#define barDimensions (Vector2){Radius * 2, 10}

Spiraler::Spiraler(Vector2 position) : Enemy(position), state(approaching) {
  Radius = DefaultRadius;
  healthManager->setMaxHealth(startingHealth);
  shotTime = 1.0f / 10.0f;
  targetPosition = Vector2Zero();
}

void Spiraler::Render() {
  healthManager->getBar()->Render();
  DrawCircleV(Position, Radius, Colour);
}

void Spiraler::Process(float delta) {
  manageHealthBar(Radius);
  spinTime += delta; 
  if(state == spinning) {
    if(fmodf(spinTime, shotTime) < 1.0f / 60.0f)
      for(int i = 0; i < bulletsPerShot; i++)
        getRoot()->addChild(new EnemyBullet(Position, spinTime * spiralSpeed + (2 * M_PI * i / bulletsPerShot), Colour, true, bulletLifetime));
    if(spinTime > SpinLength) {
      swapToApproaching();
      state = approaching;
    }
  } else if(state == approaching) {
    //here we wanna approach the target point :)
    //get vector to the point
    Vector2 vectorToPoint = Border::getShortestPathToPoint(this, targetPosition);
    //then we scale this to speed
    Vector2 velToAdd = Vector2Scale(Vector2Normalize(vectorToPoint), speed * delta);
    Velocity = Vector2Add(Velocity, velToAdd);
    //then if we're close enough to our target pos we break
    if(Vector2DistanceSqr(Position, targetPosition) <= 100 * 100) {
      swapToSpinning();
      state = spinning;
    }
  }
  Border::wrapEntity(this);
  Velocity = Vector2Scale(Velocity, friction * delta);
  Position = Vector2Add(Position, Vector2Scale(Velocity, delta));
}

Vector2 Spiraler::getTargetPosition() {
  return targetPosition;
}

void Spiraler::swapToApproaching() {
  spinTime = 0;
  targetPosition = getClosestPointToPlayerWithDistance(500);
}

void Spiraler::swapToSpinning() {
  spinTime = 0;
}

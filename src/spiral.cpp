#include "spiral.hpp"
#include "enemy.hpp"
#include "healthManager.hpp"
#include "border.hpp"
#include "sniper.hpp"
#include <cmath>
#include <cstdlib>

Color Spiraler::defaultColour = BLUE;
float Spiraler::SpinLength = 5;
float Spiraler::SpinSpeed = 10;
float Spiraler::DefaultRadius = 100;
float Spiraler::spiralSpeed = M_PI / 3.0f;
float Spiraler::startingHealth = 3;
float Spiraler::maxTargetDistance = 770;
float Spiraler::minTargetDistance = 600;
float Spiraler::speed = 2100;
float Spiraler::bulletLifetime = .5;
float Spiraler::bulletLifetimeGrowth = .15;

#define bulletsPerShot 3

#define barDimensions (Vector2){Radius * 2, 10}

Spiraler::Spiraler(Vector2 position) : Enemy(position) {
  setState(approaching);
  radius = DefaultRadius;
  getHealthManager()->setMaxHealth(startingHealth);
  shotTime = 1.0f / 10.0f;
  targetPosition = getClosestPointToPlayerWithDistance(500);
  colour = defaultColour;
}

void Spiraler::manageStates(float delta) {
  spinTime += delta; 
  if(getState() == spinning) {
    if(fmodf(spinTime, shotTime) < 1.0f / 60.0f)
      for(int i = 0; i < bulletsPerShot; i++)
        fireBullet(spinTime * spiralSpeed + (2 * M_PI * i / bulletsPerShot), bulletLifetime + spinTime * bulletLifetimeGrowth, colour);
    if(spinTime > SpinLength) {
      swapToApproaching();
      setState(approaching);
    }
  } else if(getState() == approaching) {
    //here we wanna approach the target point :)
    //get vector to the point
    Vector2 vectorToPoint = Border::getShortestPathToPoint(this, targetPosition);
    //then we scale this to speed
    Vector2 velToAdd = Vector2Scale(Vector2Normalize(vectorToPoint), speed * delta);
    velocity = Vector2Add(velocity, velToAdd);
    //then if we're close enough to our target pos we break
    if(Vector2DistanceSqr(Position, targetPosition) <= 100 * 100) {
      swapToSpinning();
      setState(spinning);
    }
  }
}

void Spiraler::dropHealth() {
  dropHealthPack();
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

#include "sniper.hpp"
#include "enemy.hpp"
#include "border.hpp"
#include <algorithm>
#include <cmath>
#include <stdio.h>

float Sniper::minDist = Border::Length / 1.6f;
float Sniper::maxDist = Border::Length * 1.4;
float Sniper::rotationSpeed = M_PI;
float Sniper::speed = 2600;
float Sniper::defaultHealth = 4;
float Sniper::maxDistFromTargetPos = 4;
float Sniper::aimTime = 4;
float Sniper::widthGrowthRate = 40;
Color Sniper::defaultColour = PURPLE;

Sniper::Sniper(Vector2 pos) : Enemy(pos), rotation(0), las(new Laser(Position)) {
  setState(positioning);
  addChild(las);
  getHealthManager()->setMaxHealth(defaultHealth);
  getHealthManager()->setHealth(defaultHealth);
  las->length = 1000;
  Colour = defaultColour;
}

Sniper::~Sniper() {}

//gur getters :p
//static
float Sniper::getMinDist() { return minDist; }
float Sniper::getMaxDist() { return maxDist; }
float Sniper::getRotationSpeed() { return rotationSpeed; }
float Sniper::getSpeed() { return speed; }
float Sniper::getDefaultHealth() { return defaultHealth; }
float Sniper::getAimTime() { return aimTime; }
float Sniper::getWidthGrowthRate() { return widthGrowthRate; }
Color Sniper::getDefaultColour() { return defaultColour; }
//regular
float Sniper::getRotation() const { return rotation; }
float Sniper::getRotationalVelocity() const { return rotationalVelocity; }
Vector2 Sniper::getTargetPosition() const { return targetPosition; }

void Sniper::manageStates(float delta) {
  //so here it's pretty easy
  {
    //this scope manages rotation
    float plrAngle = getAngleToPlayer();
    rotation = plrAngle;
    las->rotation = plrAngle;
    las->length = Vector2Length(getShortestVectorToPlayer());
  }
  las->Position = Position;
  if(getState() == positioning) {
    targetPosition = getClosestPointToPlayerWithDistance(std::clamp(Border::getDistance(Position, getPlayer()->Position), minDist, maxDist));
    Vector2 vectorToTarget = Border::getShortestPathToPoint(this, targetPosition);
    Vector2 velToAdd = Vector2Scale(Vector2Normalize(vectorToTarget), speed * delta);
    Velocity = Vector2Add(Velocity, velToAdd);
    las->shouldRender = false;
    //get if close enough to target
    if(Vector2Distance(Position, targetPosition) < maxDistFromTargetPos)
      setState(aiming);
  } else if(getState() == aiming) {
    if(las->length > maxDist)
      setState(positioning);
    las->shouldRender = true;
    las->width = widthGrowthRate * getStateTime();
  } else {
    printf("Current state is invalid with value %d on Sniper", getState());
  }
}

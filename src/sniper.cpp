#include "sniper.hpp"
#include "enemy.hpp"
#include "border.hpp"
#include <algorithm>
#include <stdio.h>

float Sniper::minDist = Border::Length / 2.0f;
float Sniper::maxDist = Border::Length / 1.2f;
float Sniper::rotationSpeed = M_PI / 2.0f;
float Sniper::speed = 2600;
Color Sniper::defaultColour = PURPLE;

Sniper::Sniper(Vector2 pos) : Enemy(pos), rotation(0), las(new Laser(Position)) {
  setState(positioning);
  addChild(las);
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
Color Sniper::getDefaultColour() { return defaultColour; }
//regular
float Sniper::getRotation() const { return rotation; }
float Sniper::getStateTime() const { return stateTime; }
Vector2 Sniper::getTargetPosition() const { return targetPosition; }

void Sniper::manageStates(float delta) {
  //so here it's pretty easy
  las->rotation = getAngleToPlayer();
  las->length = Vector2Length(getShortestVectorToPlayer()) * 50;
  las->Position = Position;
  if(getState() == positioning) {
    targetPosition = getClosestPointToPlayerWithDistance(std::clamp(Border::getDistance(Position, getPlayer()->Position), minDist, maxDist));
    Vector2 vectorToTarget = Border::getShortestPathToPoint(this, targetPosition);
    Vector2 velToAdd = Vector2Scale(Vector2Normalize(vectorToTarget), speed * delta);
    Velocity = Vector2Add(Velocity, velToAdd);
  } else if(getState() == aiming) {
  } else {
    printf("Current state is invalid with value %d on Sniper", getState());
  }
}

void Sniper::Render() {
  DrawCircleV(Position, Radius, Colour);
  DrawCircleV(targetPosition, 10, WHITE);
}

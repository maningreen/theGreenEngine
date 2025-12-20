#include "sniper.hpp"
#include "enemy.hpp"
#include "border.hpp"
#include "engine/entity.hpp"
#include "healthPack.hpp"
#include <algorithm>
#include <cmath>
#include <stdio.h>

float Sniper::minDist = Border::length / 1.6f;
float Sniper::maxDist = Border::length * 1.4;
float Sniper::rotationSpeed = M_PI;
float Sniper::speed = 2600;
float Sniper::defaultHealth = 2;
float Sniper::maxDistFromTargetPos = 4;
float Sniper::aimTime = 3;
float Sniper::widthGrowthRate = 10;
float Sniper::shotRecoil = 2000;
float Sniper::bulletSpeed = 5000;
float Sniper::bulletDamage = 2;
float Sniper::radius = 50;
Color Sniper::defaultColour = PURPLE;
Color Sniper::laserColour = {PURPLE.r, PURPLE.g, PURPLE.b, 30};

Sniper::Sniper(Vector2 pos) : Enemy(pos), rotation(0), las(new Laser(position, 0, 0, laserColour)) {
  setState(positioning);
  addChild(las);
  getHealthManager()->setMaxHealth(defaultHealth);
  getHealthManager()->setHealth(defaultHealth);
  las->length = 1000;
  radius = getRadius();
  colour = PURPLE;
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
float Sniper::getShotRecoil() { return shotRecoil; }
float Sniper::getBulletSpeed() { return bulletSpeed; }
float Sniper::getWidthGrowthRate() { return widthGrowthRate; }
float Sniper::getRadius() { return radius; }
Color Sniper::getDefaultColour() { return defaultColour; }
Color Sniper::getLaserColour() { return laserColour; }
//regular
float Sniper::getRotation() const { return rotation; }
float Sniper::getRotationalVelocity() const { return rotationalVelocity; }
Vector2 Sniper::getTargetPosition() const { return targetPosition; }

void Sniper::manageStates(float delta) {
  //so here it's pretty easy
  las->lookAt(getPlayer()->position);
  las->position = position;
  if(getState() == positioning) {
    targetPosition = getClosestPointToPlayerWithDistance(
      std::clamp(
        Border::getDistance(
            position, getPlayer(
          )->position
        ), 
        minDist, 
        maxDist
      )
    );
    Vector2 vectorToTarget = Border::getShortestPathToPoint(this, targetPosition);
    Vector2 velToAdd = Vector2Scale(Vector2Normalize(vectorToTarget), speed * delta);
    velocity = Vector2Add(velocity, velToAdd);
    las->shouldRender = false;
    //get if close enough to target
    if(Vector2Distance(position, targetPosition) < maxDistFromTargetPos)
      setState(aiming);
  } else if(getState() == aiming) {
    if(las->length < minDist)
      setState(positioning);
    las->shouldRender = true;
    las->width = widthGrowthRate * getStateTime();
    if(getStateTime() > aimTime) {
      setState(positioning);
      velocity = Vector2Add(velocity, (Vector2){-cosf(rotation) * shotRecoil, -sinf(rotation) * shotRecoil});
      fireBullet(rotation, 5, bulletSpeed, bulletDamage, colour);
    }
  } else {
    printf("Current state is invalid with value %d on Sniper", getState());
  }
}

void Sniper::dropHealth() {
  dropHealthPack();
}

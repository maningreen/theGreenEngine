#include "dasher.hpp"
#include "enemy.hpp"
#include "include.h"
#include "playerBullet.hpp"
#include "border.hpp"
#include <cstdlib>
#include <iostream>

Colour Dasher::defaultCol = BROWN;
float Dasher::speed = 2100;
float Dasher::radius = 40;
float Dasher::windupTime = .5;
float Dasher::windupSpeed = 200;
float Dasher::targetDist = 300;
float Dasher::maximumDist = 400;
float Dasher::recoveryTime = 1;
float Dasher::recoverSpeedThreshold = 260;
float Dasher::defaultHealth = 2;
float Dasher::dashTime = .2;
float Dasher::dashSpeed = 200000;
float Dasher::damage = 4;

Dasher::Dasher(Vector2 p) : Enemy(p) {
  setState(approaching);
  healthManager->setMaxHealth(defaultHealth);
  Colour = defaultCol;
}

void Dasher::swapState(unsigned char s) {
  setState(s);
  stateTime = 0;
}

float Dasher::getDefaultHealth() {
  return defaultHealth;
}

float Dasher::getStateTime() {
  return stateTime;
}

Color Dasher::getColour() {
  return defaultCol;
}

float Dasher::getWindupTime() {
  return windupTime;
}

float Dasher::getWindupSpeed() {
  return windupSpeed;
}

float Dasher::getSpeed() {
  return speed;
}

float Dasher::getDamage() {
  return damage;
}

float Dasher::getRecoverTime() {
  return recoveryTime;
}

float Dasher::getRecoverSpeedThreshold() {
  return recoverSpeedThreshold;
}

void Dasher::Process(float delta) {
  stateTime += delta;
  //oh god here we goooooo
  if(getState() == approaching) {
    //so here what do
    //move the fuck forward.
    //i kid :p
    //we get the vector to the player
    Vector2 closestVecToPlr = getClosestPointToPlayerWithDistance(targetDist);
    //step two we cap the bagnibude
    Vector2 velToAdd = Vector2Scale(Vector2Normalize(closestVecToPlr), delta * speed);
    //step c
    //add this to vel
    Velocity = Vector2Add(Velocity, velToAdd);
    //step 5
    //if we close enough we swap states
    if(Vector2DistanceSqr(plr->Position, Position) <= maximumDist * maximumDist)
      //we close enough :D
      swapState(winding);
  } else if(getState() == recovery) {
    //we wait :p
    if(Vector2LengthSqr(Velocity) >= recoverSpeedThreshold * recoverSpeedThreshold)
      stateTime = 0;
    if(stateTime > recoveryTime)
      swapState(approaching);
  } else if(getState() == winding) {
    //i have no fuckin clue lets skip this for now
    //then we set our state vector
    stateVector = Vector2Normalize(getShortestVectorToPlayer());
    //so what we wanna do, get the shortest vector to player
    //then we scale this to windupSpeed
    Velocity = Vector2Add(Vector2Scale(Vector2Normalize(stateVector), -stateTime * windupSpeed), Velocity);
    if(stateTime > windupTime) {
      swapState(dashing);
      Velocity = Vector2Zero();
    }
  } else if(getState() == dashing) {
    //we... dash?
    Velocity = Vector2Scale(stateVector, dashSpeed * delta);
    if(stateTime > dashTime)
      swapState(recovery);
    //we also gotta check some collision shtuff
    if(Vector2DistanceSqr(Position, plr->Position) < (Player::hitboxRadius + radius) * (Player::hitboxRadius + radius)) {
      //for psuedo i-frames
      plr->getHealthManager()->applyDamage(damage);
      swapState(recovery);
    }
  } else
    std::cout << "WHAT THE FUCK YOUR STATE IS SHIT MAN HOW THE HELL DO YOU DO THIS\n(dasher)";
  // :D
  Position = Vector2Add(Position, Vector2Scale(Velocity, delta));
  Velocity = Vector2Scale(Velocity, friction * delta);
  Border::wrapEntity(this);
  manageHealthBar(radius);
}

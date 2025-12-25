#include "dasher.hpp"
#include "enemy.hpp"
#include "player.hpp"
#include <cstdlib>
#include <iostream>

Color Dasher::defaultCol = BROWN;
float Dasher::speed = 2100;
float Dasher::radius = 40;
float Dasher::windupTime = .5;
float Dasher::windupSpeed = 200;
float Dasher::targetDist = 300;
float Dasher::maximumDist = 400;
float Dasher::recoveryTime = 1;
float Dasher::recoverSpeedThreshold = 260;
float Dasher::defaultHealth = 1;
float Dasher::dashTime = .5;
float Dasher::dashSpeed = 200000;
float Dasher::damage = 4;
float Dasher::healthDropChance = 3;

Dasher::Dasher(Vector2 p) : Enemy(p) {
  setState(approaching);
  getHealthManager()->setMaxHealth(defaultHealth);
  colour = BROWN;
}

float Dasher::getDefaultHealth() { return defaultHealth; }

Color Dasher::getColour() { return defaultCol; }

float Dasher::getWindupTime() { return windupTime; }

float Dasher::getWindupSpeed() { return windupSpeed; }

float Dasher::getSpeed() { return speed; }

float Dasher::getDamage() { return damage; }

float Dasher::getRecoverTime() { return recoveryTime; }

float Dasher::getRecoverSpeedThreshold() { return recoverSpeedThreshold; }

float Dasher::getHealthDropChance() { return healthDropChance; }

void Dasher::manageStates(float delta) {
  // oh god here we goooooo
  if(getState() == approaching) {
    // so here what do
    // move the fuck forward.
    // i kid :p
    // we get the vector to the player
    Vector2 closestVecToPlr = getClosestPointToPlayerWithDistance(targetDist);
    // step two we cap the bagnibude
    Vector2 velToAdd = Vector2Scale(
        Vector2Normalize(Vector2Subtract(closestVecToPlr, position)),
        delta * speed);
    // step c
    // add this to vel
    velocity = Vector2Add(velocity, velToAdd);
    // step 5
    // if we close enough we swap states
    if(Vector2DistanceSqr(getPlayer()->position, position) <=
        maximumDist * maximumDist)
      // we close enough :D
      setState(winding);
  } else if(getState() == recovery) {
    // we wait :p
    if(Vector2LengthSqr(velocity) >=
        recoverSpeedThreshold * recoverSpeedThreshold)
      resetStateTime();
    if(getStateTime() > recoveryTime)
      setState(approaching);
  } else if(getState() == winding) {
    // then we set our state vector
    stateVector = Vector2Normalize(getShortestVectorToPlayer());
    // so sniper what we wanna do, get the shortest vector to player
    // then we scale this to windupSpeed
    velocity = Vector2Add(Vector2Scale(Vector2Normalize(stateVector),
                              -getStateTime() * windupSpeed),
        velocity);
    if(getStateTime() > windupTime) {
      setState(dashing);
      velocity = Vector2Zero();
    }
  } else if(getState() == dashing) {
    // we... dash?
    velocity = Vector2Scale(stateVector, dashSpeed * delta);
    if(getStateTime() > dashTime)
      setState(recovery);
    // we also gotta check some collision shtuff
    if(Vector2DistanceSqr(position, getPlayer()->position) <
        (Player::hitboxRadius + radius) * (Player::hitboxRadius + radius)) {
      // for psuedo i-frames
      ((Player*)getPlayer())->getHealthManager()->applyDamage(damage);
      setState(recovery);
      // then we do a bounce
      Vector2 scaledOffset =
          Vector2Normalize(Vector2Subtract(getPlayer()->position, position));
      scaledOffset = {scaledOffset.x > 0 ? -scaledOffset.x : scaledOffset.x,
          scaledOffset.y > 0 ? -scaledOffset.y : scaledOffset.y};
      velocity =
          (Vector2) {scaledOffset.x * velocity.x, scaledOffset.y * velocity.y};
    }
  } else
    std::cout << "WHAT THE FUCK YOUR STATE IS SHIT MAN HOW THE HELL DO YOU DO "
              << "THIS\n(dasher)";
  // :D
}

void Dasher::dropHealth() {
  // step one:
  // random number
  int r = rand() % (int)(healthDropChance);
  if(r == 0) // means, it's healthDropChance deep deep down
    dropHealthPack();
}

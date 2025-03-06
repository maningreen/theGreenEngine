#include "dasher.hpp"
#include "enemy.hpp"
#include "include.h"
#include "playerBullet.hpp"
#include "border.hpp"
#include <iostream>

Colour Dasher::defaultCol = BROWN;
float Dasher::speed = 2100;
float Dasher::radius = 40;
float Dasher::windupTime = .5;
float Dasher::targetDist = 300;
float Dasher::maximumDist = 400;
float Dasher::recoveryTime = 2;

Dasher::Dasher(Vector2 p) : Enemy(p) {
  setState(approaching);
  Colour = defaultCol;
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

float Dasher::getSpeed() {
  return speed;
}

void Dasher::swapState() {
  stateTime = 0;
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
    if(Vector2LengthSqr(closestVecToPlr) > speed * speed)
      closestVecToPlr = Vector2Scale(Vector2Normalize(closestVecToPlr), speed * delta);
    //step c
    //add this to vel
    Velocity = Vector2Add(Velocity, closestVecToPlr);
    //step 5
    //if we close enough we swap states
    if(Vector2DistanceSqr(plr->Position, Position) >= maximumDist * maximumDist)
      goto lbl;
    //we close enough :D
    swapState();
    setState(winding);
    goto lbl;
  } else if(getState() == recovery) {
    //we wait :p
    if(stateTime <= recoveryTime)
      goto lbl;
    setState(approaching);
    swapState();
    goto lbl;
  } else if(getState() == winding) {
    //i have no fuckin clue lets skip this for now
    setState(dashing);
    swapState();
  } else if(getState() == dashing) {
  } else {
    std::cout << "WHAT THE FUCK YOUR STATE IS SHIT MAN HOW THE HELL DO YOU DO THIS\n(dasher)";
  }
lbl:
  // :D
  Position = Vector2Add(Position, Vector2Scale(Velocity, delta));
  Velocity = Vector2Scale(Velocity, friction * delta);
  Border::wrapEntity(this);
  manageHealthBar(radius);
}

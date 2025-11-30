#include "dashManager.hpp"

#include "raylib.h"
#include "raymath.h"

#include <cmath>
#include <cstdio>

DashManager::DashManager(unsigned dashCount,
  float dashLength,
  float dashRegenDelay,
  float dashControl,
  float dashSpeed)
  : maxDashCount(dashCount), length(dashLength),
    regenDelay(dashRegenDelay), control(dashControl),
    speed(dashSpeed) {
  progress = 0;
  regenRate = 1;
  dashing = false;
  deltaDash = 0;
}

DashManager::~DashManager() {}

int DashManager::beginDash(Vector2 dir) {
  if(isDashing())
    return 1;
  else if(!getAvailableDashes())
    return 2;

  dashing = true;
  deltaDash = 0;
  dashVelocity = Vector2Scale(dir, speed);

  return 0;
}

int DashManager::getAvailableDashes() { return floorf(progress); }

bool DashManager::canDash() { return progress >= 1 && !isDashing(); }

bool DashManager::isDashing() { return dashing; }

float DashManager::getDashProgress() { return progress; }

void DashManager::addDashProgress() { 
  progress += 1;
  if(progress >= maxDashCount)
    progress = maxDashCount;
}

float DashManager::getNextDashProgress(float delta) {
  float next = progress + regenRate * delta;
  if(next >= maxDashCount)
    return maxDashCount;
  else
    return next;
}

void DashManager::removeDashProgress() {
  if(getAvailableDashes())
    --progress;
  deltaDash = 0;
}

Vector2 DashManager::getDashDirection() {
  return Vector2Normalize(dashVelocity);
}

Vector2 DashManager::getDashVelocity() { 
  return isDashing() ? dashVelocity : Vector2Zero(); 
}

Vector2 DashManager::applyInput(float delta, Vector2 i) {
  Vector2 normalizedVel = Vector2Normalize(dashVelocity);
  Vector2 scaledInput = Vector2Scale(i, delta * control);
  return Vector2Scale(normalizedVel + scaledInput, speed);
}

float DashManager::getDeltaDash() {
  return deltaDash;
}

Vector2 DashManager::manageDash(float delta, Vector2 inputVector) {
  deltaDash += delta;
  if(deltaDash > regenDelay + length)
    progress = getNextDashProgress(delta);
  else if(isDashing()) {
    dashVelocity = applyInput(delta, inputVector);
    dashing = deltaDash < length;
  }

  return dashVelocity;
}

Vector2 DashManager::getPositionIncrement(float delta) {
  return Vector2Scale(dashVelocity, delta);
}

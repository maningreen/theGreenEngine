#include "dashManager.hpp"

#include "raylib.h"
#include "raymath.h"

#include <cmath>
#include <cstdio>

DashManager::DashManager(unsigned dashCount,
  float dashLength,
  float dashRegenDelay,
  float dashControl,
  float dashSpeed,
  float cooldown)
  : maxDashCount(dashCount), dashLength(dashLength),
    dashRegenDelay(dashRegenDelay), dashControl(dashControl),
    dashSpeed(dashSpeed), dashCooldown(cooldown) {
  dashProgress = 0;
  dashRegenRate = 1;
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
  dashVelocity = Vector2Scale(dir, dashSpeed);

  return 0;
}

int DashManager::getAvailableDashes() { return floorf(dashProgress); }

bool DashManager::canDash() { return dashProgress >= 1 && !isDashing(); }

bool DashManager::isDashing() { return dashing; }

float DashManager::getDashProgress() { return dashProgress; }

float DashManager::getNextDashProgress(float delta) {
  float next = dashProgress + dashRegenRate * delta;
  printf("%f\n", dashProgress);
  if(next >= maxDashCount)
    return maxDashCount;
  else
    return next;
}

void DashManager::removeDashProgress() {
  if(getAvailableDashes())
    --dashProgress;
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
  Vector2 scaledInput = Vector2Scale(i, delta * dashControl);
  return Vector2Scale(normalizedVel + scaledInput, dashSpeed);
}

float DashManager::getDeltaDash() {
  return deltaDash;
}

Vector2 DashManager::manageDash(float delta, Vector2 inputVector) {
  deltaDash += delta;
  if(deltaDash > dashRegenDelay + dashLength)
    dashProgress = getNextDashProgress(delta);
  else if(isDashing()) {
    dashVelocity = applyInput(delta, inputVector);
    if(deltaDash >= dashLength)
       dashing = false;
  }

  return dashVelocity;
}

Vector2 DashManager::getPositionIncrement(float delta) {
  return Vector2Scale(dashVelocity, delta);
}

#include "dashManager.hpp"

#include "raylib.h"
#include "raymath.h"

#include <cmath>

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
  deltaDash = 0;
}

DashManager::~DashManager() {}

int DashManager::beginDash(Vector2 dir) {
  if(isDashing())
    return 1;
  else if(!getAvailableDashes())
    return 2;
  deltaDash = 0;
  return 0;
}

int DashManager::getAvailableDashes() { return floorf(dashProgress); }

bool DashManager::isDashing() { return deltaDash <= dashLength; }

float DashManager::getNextDashProgress(float delta) {
  float next = dashProgress + dashRegenRate * delta;
  if(next >= maxDashCount)
    return maxDashCount;
  else
    return next;
}

void DashManager::removeDashProgress() {
  if(getAvailableDashes())
    --dashProgress;
}

Vector2 DashManager::getDashDirection() {
  return Vector2Normalize(dashVelocity);
}

Vector2 DashManager::getDashVelocity() { return dashVelocity; }

Vector2 DashManager::applyInput(Vector2 i) {
  Vector2Scale(Vector2Normalize(dashVelocity + Vector2Scale(i, dashSpeed * dashControl)), dashSpeed);
  Vector2 normalizedVel = Vector2Normalize(dashVelocity);
  Vector2 scaledI = Vector2Scale(i, dashControl);
  return Vector2Scale(normalizedVel + scaledI, dashSpeed);
}

float DashManager::getDeltaDash() {
  return deltaDash;
}

Vector2 DashManager::manageDash(float delta, Vector2 inputVector) {
  deltaDash += delta;
  if(deltaDash > dashRegenDelay + dashLength)
    dashProgress = getNextDashProgress(delta);
  else if(isDashing())
    dashVelocity = applyInput(inputVector);
  return getPositionIncrement(delta);
}

Vector2 DashManager::getPositionIncrement(float delta) {
  return Vector2Scale(dashVelocity, delta);
}

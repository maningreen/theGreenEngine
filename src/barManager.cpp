#include "barManager.hpp"
#include "bars.hpp"
#include "engine/entity.hpp"
#include "include.h"
#include "iostream"


BarManager::BarManager(Vector2 Position, float targetDistance, Bar childBar) : Entity2D("BarManager", Position), bar(childBar), targetDistance(targetDistance), positionPointer(0) {
}

BarManager::BarManager(Vector2 *Position, float targetDistance, Bar childBar) : Entity2D("BarManager", *Position), bar(childBar), targetDistance(targetDistance) {
}

BarManager::~BarManager() {
}

Bar* BarManager::getBar() {
  return &bar;
}

Vector2* BarManager::getPositionPointer() {
  return positionPointer;
}

void BarManager::Process(float delta) {
  if(!positionPointer) //should be 0 if not real
    return;
  // now we know we gots to set our position
  Position = Vector2Subtract(Vector2Add(*positionPointer, !bar.ShrinkY ? (Vector2){0, targetDistance} : (Vector2){targetDistance, 0}), !bar.ShrinkY ? (Vector2){ bar.Dimensions.x / 2.0f, 0} : (Vector2){0, bar.Dimensions.y / 2.0f});
  bar.Position = Position;
  bar.ShouldRender = true;
}

void BarManager::setBarPercentage(float p) {
  bar.TargetProgress = p;
}

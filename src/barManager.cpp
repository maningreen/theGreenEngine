#include "barManager.hpp"
#include "bars.hpp"
#include "entity.hpp"
#include "include.h"


BarManager::BarManager(Vector2 Position, float targetDistance, Bar *childBar) : Entity2D("BarManager", Position), bar(childBar), targetDistance(targetDistance), positionPointer(0) {
  addChild(childBar);
}

BarManager::BarManager(Vector2 *Position, float targetDistance, Bar *childBar) : Entity2D("BarManager", *Position), bar(childBar), targetDistance(targetDistance) {}

BarManager::~BarManager() {}

Bar* BarManager::getBar() {
  return bar;
}

void BarManager::Process(float delta) {
  if(!positionPointer) //should be 0 if not real
    return;
  // now we know we gots to set our position
  bar->Position = Vector2Add(Position, (Vector2){bar->ShrinkY ? targetDistance : 0, !bar->ShrinkY ? targetDistance : 0});
}

void BarManager::setBarPercentage(float p) {
  bar->TargetProgress = p;
}

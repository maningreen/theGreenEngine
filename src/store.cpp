#include "store.hpp"
#include "stdio.h"
#include "enemy.hpp"
#include "player.hpp"
#include "engine/core.h"

template <typename t>
Vector2 StoreItem<t>::stdDimensions = (Vector2){700, 200};
template <typename t>
float StoreItem<t>::borderWidth = 30;
template <typename t>
float StoreItem<t>::upgradePercent = 10;
template <typename t>
float StoreItem<t>::purchaseTime = 1;

template <typename t>
StoreItem<t>::StoreItem(std::string name, t* ptr) :  ptr(ptr), Entity2D(name, Vector2Zero()), purchaseProgress(0) {}
template <>
StoreItem<float>::StoreItem(std::string name, float* ptr) :  ptr(ptr), Entity2D(name, Vector2Zero()), purchaseProgress(0) {}

template <typename t>
StoreItem<t>::~StoreItem() {}

template <typename t>
void StoreItem<t>::Render() {
  DrawRectangleV(Position, stdDimensions, BLACK);
  DrawRectangleLinesEx((Rectangle){Position.x, Position.y, stdDimensions.x, stdDimensions.y}, borderWidth, WHITE);
  {
    //here we do some magic shtuff to center the text
    Vector2 fontDems = MeasureTextEx(GetFontDefault(), Name.c_str(), 100, 1);
    Vector2 offset = Vector2Scale(Vector2Subtract(stdDimensions, fontDems), .5);
    DrawText(Name.c_str(), Position.x + offset.x, Position.y + offset.y, 100, WHITE);
  }
  //then we draw progress
  DrawRectangleV(Position, (Vector2){stdDimensions.x * purchaseProgress, stdDimensions.y}, WHITE);
}

template <typename t>
void StoreItem<t>::Process(float delta) {
  if(Enemy::getPlayer() == nullptr)
    return;
  if(CheckCollisionCircleRec(Enemy::getPlayer()->Position, Player::hitboxRadius, (Rectangle){Position.x, Position.y, stdDimensions.x, stdDimensions.y})) {
    purchaseProgress += delta / purchaseTime;
    if(purchaseProgress > 1) {
      upgrade(upgradePercent);
      killDefered();
    }
  } else
    purchaseProgress -= delta / purchaseTime;
  purchaseProgress = purchaseProgress < 0 ? 0 : purchaseProgress;
}

template <typename t>
void StoreItem<t>::StoreItem::upgrade(float perc) {
  purchaseProgress = 0;
  *ptr += *ptr * perc / 100;
}

template <typename t>
Vector2 StoreItem<t>::getStdDimensions() { return stdDimensions; }
template <typename t>
float StoreItem<t>::getBorderWidth() { return borderWidth; }
template <typename t>
float StoreItem<t>::getPurchaseProgress() { return purchaseProgress; }

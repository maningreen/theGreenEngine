#include "store.hpp"
#include "player.hpp"

template <typename t>
void UpgradeManager<t>::upgrade(t *ptr, float upCount) {
  *ptr += upCount;
}

template <typename t>
Vector2 StoreItem<t>::stdDimensions = (Vector2){1000, 200};
template <typename t>
float StoreItem<t>::borderWidth = 30;

template <typename t>
StoreItem<t>::StoreItem(std::string name, t* ptr) :  ptr(ptr), Entity2D(name, Vector2Zero()) {}
template <>
StoreItem<float>::StoreItem(std::string name, float* ptr) :  ptr(ptr), Entity2D(name, Vector2Zero()) {}

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
  //:wq
  //
}

template <typename t>
void StoreItem<t>::Process(float delta) {}

template <typename t>
Vector2 StoreItem<t>::getStdDimensions() { return stdDimensions; }
template <typename t>
float StoreItem<t>::getBorderWidth() { return borderWidth; }

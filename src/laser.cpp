#include "laser.hpp"
#include "engine/entity.hpp"
#include "border.hpp"
#include <stdio.h>

Color Laser::defaultColour = WHITE;
float Laser::defaultLength = 100;
float Laser::defaultWidth = 20;

Laser::Laser(Vector2 p, float rot, float len, Color col) : Entity2D("Laser", p), rotation(rot), length(len), colour(col), width(defaultWidth) {}
Laser::Laser(Vector2 p, float rot, float len) : Entity2D("Laser", p), rotation(rot), length(len), colour(defaultColour), width(defaultWidth) {}
Laser::Laser(Vector2 p, float rot) : Entity2D("Laser", p), rotation(rot), length(defaultLength), colour(defaultColour), width(defaultWidth) {}
Laser::Laser(Vector2 p) : Entity2D("Laser", p), rotation(0), length(defaultLength), colour(defaultColour), width(defaultWidth) {}

Color Laser::getDefaultColour() {
  return defaultColour;
}

float Laser::getDefaultLength() {
  return defaultLength;
}

float Laser::getDefaultWidth() {
  return defaultWidth;
}

void Laser::Render() {
  Vector2 localOffset = (Vector2){cosf(rotation) * length, sinf(rotation) * length};
  Vector2 rayOrigin = Position;
  Vector2 preWrap = Vector2Add(rayOrigin, localOffset);
  Vector2 endPos = Border::wrapPos(preWrap);

  if(preWrap == endPos)
    DrawLineEx(Position, endPos, width, colour);
  else while(abs(preWrap.x - endPos.x) > 10 || abs(preWrap.y - endPos.y) > 10) {
    //get intersection
    bool left = endPos.x < 0;
    bool top = endPos.y < 0;

    bool swapX = preWrap.x - Border::Length > preWrap.y - Border::Length;
    
    float originPostX = endPos.x + (swapX ? (left ? Border::Length : -Border::Length) : 0);
    float originPostY = endPos.y + (!swapX ? (top ? Border::Length : -Border::Length) : 0);

    float percX = originPostX / localOffset.x;
    float percY = originPostY / localOffset.y;

    Vector2 vectorToCollision = Vector2Scale(localOffset, 1 - (swapX ? percX : percY));
    Vector2 collisionPosition = Vector2Add(rayOrigin, vectorToCollision);

    DrawLineEx(rayOrigin, collisionPosition, width, GREEN);
    
    localOffset = Vector2Subtract(localOffset, vectorToCollision);
    rayOrigin = {swapX ? -collisionPosition.x : collisionPosition.x, !swapX ? -collisionPosition.y : collisionPosition.y};
    preWrap = Vector2Add(rayOrigin, localOffset);
  }
  DrawLineEx(rayOrigin, endPos, width, colour);
}

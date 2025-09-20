#include "laser.hpp"
#include "engine/entity.hpp"
#include "border.hpp"
#include <cmath>
#include <raymath.h>

#define cot(x) (1 / tan(x))

Color Laser::defaultColour = WHITE;
float Laser::defaultLength = 100;
float Laser::defaultWidth = 20;

Laser::Laser(Vector2 p, float rot, float len, Color col) : Entity2D("Laser", p), rotation(rot), length(len), colour(col), shouldRender(true), width(defaultWidth) {}
Laser::Laser(Vector2 p, float rot, float len) : Entity2D("Laser", p), rotation(rot), length(len), colour(defaultColour), shouldRender(true), width(defaultWidth) {}
Laser::Laser(Vector2 p, float rot) : Entity2D("Laser", p), rotation(rot), length(defaultLength), colour(defaultColour), shouldRender(true), width(defaultWidth) {}
Laser::Laser(Vector2 p) : Entity2D("Laser", p), rotation(0), length(defaultLength), colour(defaultColour), shouldRender(true), width(defaultWidth) {}

Color Laser::getDefaultColour() {
  return defaultColour;
}

float Laser::getDefaultLength() {
  return defaultLength;
}

float Laser::getDefaultWidth() {
  return defaultWidth;
}

void Laser::lookAt(Vector2 v) {
  Vector2 vecTo = Border::getShortestPathToPoint(Position, v);
  rotation = atan2f(vecTo.y, vecTo.x);
  length = Vector2Length(vecTo);
}

bool Laser::getBreaks() {
  return getBreaksX() || getBreaksY();
}

bool Laser::getBreaksX() {
  Vector2 localOffset = (Vector2){cosf(rotation) * length, sinf(rotation) * length};
  Vector2 preWrap = Vector2Add(Position, localOffset);
  Vector2 endPos = Border::wrapPos(preWrap);
  return endPos.x != preWrap.x;
}

bool Laser::getBreaksY() {
  Vector2 localOffset = (Vector2){cosf(rotation) * length, sinf(rotation) * length};
  Vector2 preWrap = Vector2Add(Position, localOffset);
  Vector2 endPos = Border::wrapPos(preWrap);
  return endPos.y != preWrap.y;
}

Vector2 Laser::getBreakingPoint() {
  if(!getBreaks())
    return Vector2Zero();
  Vector2 endPos = getEndPoint();
  float slope = (endPos.y - Position.y) / (endPos.x - Position.x);
  float xLen, yLen;
  if(getBreaksX()) {
    xLen = Border::Length - abs(Position.x);
    yLen = xLen * slope + Position.y;
  } else if(getBreaksY()) {
    yLen = Border::Length - abs(Position.y);
    xLen = xLen / slope + Position.y;
  }
  return (Vector2){ Position.x + xLen, Position.y + yLen};
}

Vector2 Laser::getEndPoint() {
  Vector2 localOffset = (Vector2){cosf(rotation) * length, sinf(rotation) * length};
  return Border::wrapPos(Vector2Add(Position, localOffset));
}

void Laser::Render() {
  if(!shouldRender) //:)
    return;
  Vector2 localOffset = (Vector2){cosf(rotation) * length, sinf(rotation) * length};
  Vector2 rayOrigin = Position;
  Vector2 preWrap = Vector2Add(rayOrigin, localOffset);
  Vector2 endPos = Border::wrapPos(preWrap);

  int i = 0;

  if(preWrap.x == endPos.x && preWrap.y == endPos.y)
    DrawLineEx(Position, endPos, width, colour);
  else while(abs(preWrap.x - endPos.x) > 10 || abs(preWrap.y - endPos.y) > 10) {
      //get intersection
      bool left = localOffset.x >= 0;
      bool top = localOffset.y >= 0;

      Vector2 collisionPosition;
      float slope = tan(rotation);
      float yPosAtBorder = rayOrigin.y + (slope * ((left ? Border::Length  : -Border::Length) - rayOrigin.x));
      float xPosAtBorder = rayOrigin.x + (1 / slope) * ((top ? Border::Length : -Border::Length) - rayOrigin.y);
      bool swapX = abs(yPosAtBorder) <= Border::Length;
      bool swapY = abs(xPosAtBorder) <= Border::Length;
      //so the gist with this variable is if we collide on x we set it to be the y it collides on and vise versa
      if(swapX) {
        collisionPosition.y = yPosAtBorder;
        collisionPosition.x = left ? Border::Length : -Border::Length;
      } 
      if(swapY) {
        collisionPosition.x = xPosAtBorder;
        collisionPosition.y = top ? Border::Length : -Border::Length;
      }
      Vector2 vectorToCol = Vector2Subtract(collisionPosition, rayOrigin);
      localOffset = Vector2Subtract(localOffset, vectorToCol);

      DrawLineEx(rayOrigin, collisionPosition, width, colour);

      rayOrigin = {swapX ? -collisionPosition.x : collisionPosition.x, !swapX ? -collisionPosition.y : collisionPosition.y};
      preWrap = Vector2Add(rayOrigin, localOffset);
    }
  DrawLineEx(rayOrigin, endPos, width, colour);
}

#include "laser.hpp"
#include "engine/entity.hpp"
#include "border.hpp"
#include <cmath>
#include <stdio.h>

#define cot(x) (cos(x) / sin(x))

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

  int i = 0;

  if(preWrap == endPos)
    DrawLineEx(Position, endPos, width, colour);
  else while(abs(preWrap.x - endPos.x) > 100 || abs(preWrap.y - endPos.y) > 100) {
    //get intersection
    bool left = localOffset.x > 0;
    bool top = localOffset.y > 0;

    bool swapX;
    Vector2 collisionPosition;
    {
      double slope = tan(rotation);
      double yPosAtBorder = rayOrigin.y + (slope * ((left ? Border::Length  : -Border::Length) - rayOrigin.x));
      DrawCircle(left ? Border::Length : -Border::Length,yPosAtBorder, 60, WHITE);
      swapX = abs(yPosAtBorder) <= Border::Length;
      if(i == 0)
        printf("%b\n", swapX);
      //so the gist with this variable is if we collide on x we set it to be the y it collides on and vise versa
      double variablePosition;
      if(swapX) {
        variablePosition = yPosAtBorder;
        collisionPosition.y = variablePosition;
        collisionPosition.x = left ? Border::Length : -Border::Length;
      }
      else {
        variablePosition = rayOrigin.x + ((cot(rotation)) * ((top ? Border::Length : -Border::Length) - rayOrigin.y));
        collisionPosition.x = variablePosition;
        collisionPosition.y = top ? Border::Length : -Border::Length;
      }
      DrawCircleV(collisionPosition, 60, GRAY);
      Vector2 vectorToCol = Vector2Subtract(collisionPosition, rayOrigin);
      localOffset = Vector2Subtract(localOffset, vectorToCol);
    }

    DrawLineEx(rayOrigin, collisionPosition, width, colour);
    
    rayOrigin = {swapX ? -collisionPosition.x : collisionPosition.x, !swapX ? -collisionPosition.y : collisionPosition.y};
    preWrap = Vector2Add(rayOrigin, localOffset);
    if(++i > 5)
      break;
  }
  DrawLineEx(rayOrigin, endPos, width, colour);
}

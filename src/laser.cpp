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
  Vector2 preWrap = Vector2Add(Position, localOffset);
  Vector2 endPos = Border::wrapPos(preWrap);

  if(preWrap != endPos) {
    //get intersection
    bool left = endPos.x < 0;
    bool top = endPos.y < 0;

    bool swapX = preWrap.x != endPos.x;
    bool swapY = preWrap.y != endPos.y;
    
    float originPostX = endPos.x + (swapX ? (left ? Border::Length : -Border::Length) : 0);
    float originPostY = endPos.y + (swapY ? (top ? Border::Length : -Border::Length) : 0);

    DrawCircleV((Vector2){originPostX, originPostY}, 50, BLUE);

    float percX = originPostX / localOffset.x;
    float percY = originPostY / localOffset.y;

    Vector2 vectorToCollision = Vector2Scale(localOffset, (1 - (swapX ? percX : percY)));
    Vector2 collisionPosition = Vector2Add(Position, vectorToCollision);

    Vector2 playerCollisionPoint = (Vector2){collisionPosition.x * (swapX ? -1 : 1), collisionPosition.y * (swapY ? -1 : 1)};

    DrawLineEx(Position, collisionPosition, width, colour);
    //DrawCircleV(collisionPosition, 50, BLUE);
    if(swapX ^ swapY)
      DrawLineEx(playerCollisionPoint, endPos, width, colour);
    else {
      Vector2 remainingVector = (Vector2){localOffset.x * percX, localOffset.y * percY};
      //what we wanna do is get whichevery has a larger x/y
      bool xLarger = localOffset.x > localOffset.y;
      printf("%d\n", xLarger);
      Vector2 wrapPos;
      Vector2 secondColPos = collisionPosition;
      if(xLarger) {
        wrapPos = Border::wrapPosX(Vector2Add(remainingVector, collisionPosition));
        secondColPos.x *= -1;
      } else {
        wrapPos = Border::wrapPosY(Vector2Add(remainingVector, collisionPosition));
        secondColPos.y *= -1;
      }
      DrawCircleV(wrapPos, 50, ORANGE);
      DrawCircleV(secondColPos, 50, ORANGE);
      DrawCircleV(remainingVector, 50, ORANGE);
      printf("%f %f\n", wrapPos.x, wrapPos.y);
    }
  } else
    DrawLineEx(Position, endPos, width, colour);
}

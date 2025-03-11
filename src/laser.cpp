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
    
    float originPreX = localOffset.x;
    float originPostX = endPos.x + (swapX ? (left ? Border::Length : -Border::Length) : 0);

    float originPreY = localOffset.y;
    float originPostY = endPos.y + (swapY ? (top ? Border::Length : -Border::Length) : 0);

    float percX = originPostX / originPreX;
    float percY = originPostY / originPreY;

    Vector2 vectorToCollision = Vector2Scale(localOffset, 1 - (swapX ? percX : percY));
    Vector2 collisionPosition = Vector2Add(Position, vectorToCollision);

    Vector2 playerCollisionPoint = (Vector2){collisionPosition.x * (swapX ? -1 : 1), collisionPosition.y * (swapY ? -1 : 1)};

    DrawLineEx(Position, collisionPosition, width, colour);
    if(swapX ^ swapY)
      DrawLineEx(playerCollisionPoint, endPos, width, colour);
    else {
      localOffset = (Vector2){cosf(rotation) * length * percX, sinf(rotation) * length * percY};
      DrawLineV(Vector2Add(localOffset, collisionPosition), collisionPosition, colour);
    }
    DrawCircleV(Border::wrapPos(collisionPosition), 50, BLUE);
  } else
    DrawLineEx(Position, endPos, width, colour);
}

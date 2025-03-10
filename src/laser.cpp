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
    
    float originPreX = preWrap.x - Position.x;
    float originPostX = endPos.x + (left ? Border::Length : -Border::Length);

    float originPreY = preWrap.y - Position.y;
    float originPostY = endPos.y + (top ? Border::Length : -Border::Length);

    float percX = abs(originPostX / originPreX);
    float percY = abs(originPostY / originPreY);

    Vector2 collisionPosition = Vector2Add(Position, (Vector2){localOffset.x * (1 - percX), localOffset.y * (1 - percY)});

    printf("%f %f\n", percX, percY);

    DrawLineEx(Position, collisionPosition, width, colour);
    DrawLineEx((Vector2){collisionPosition.x * (swapX ? -1 : 1), collisionPosition.y * (swapY ? -1 : 1)}, endPos, width, colour);
    DrawCircleV(collisionPosition, 50, BLUE);

  } else
    DrawLineEx(Position, endPos, width, colour);
}

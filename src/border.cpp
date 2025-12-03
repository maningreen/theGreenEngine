#include "border.hpp"
#include "engine/entity.hpp"
#include <cmath>
#include <unistd.h>

#define abs(x) (x < 0 ? -x : x)

float Border::length = 2000;

Border::Border() : Entity2D("Border", (Vector2){0, 0}){}

Border::~Border() {}

void Border::render() {
  DrawRectangleLinesEx((Rectangle){-length, -length, length * 2, length * 2}, 20, WHITE);
}

void Border::wrapEntity(Entity2D* en) {
  en->position = wrapPos(en->position);
}

Vector2 Border::wrapPos(Vector2 p) {
  return (Vector2){wrapPosX(p).x, wrapPosY(p).y};
}

Vector2 Border::wrapPosX(Vector2 v) {
  while(abs(v.x) > Border::length) {
    if(v.x < -length)
      v.x += length * 2;
    else if(v.x > length)
      v.x -= length * 2;
  }
  return v;
}

Vector2 Border::wrapPosY(Vector2 v) {
  while(abs(v.y) > Border::length) {
    if(v.y < -length)
      v.y += length * 2;
    else if(v.y > length)
      v.y -= length * 2;
  }
  return v;
}

Vector2 Border::getShortestPathToPoint(const Entity2D* en, const Vector2 point) {
  Vector2 vectorTo = Vector2Subtract(point, en->position);
  if(vectorTo.x < -length)
    vectorTo.x += length * 2;
  else if(vectorTo.x > length)
    vectorTo.x -= length * 2;
  if(vectorTo.y < -length)
    vectorTo.y += length * 2;
  else if(vectorTo.y > length)
    vectorTo.y -= length * 2;
  return vectorTo;
}

Vector2 Border::getShortestPathToPoint(const Vector2 a, const Vector2 b) {
  Vector2 vectorTo = Vector2Subtract(b, a);
  if(vectorTo.x < -length)
    vectorTo.x += length * 2;
  else if(vectorTo.x > length)
    vectorTo.x -= length * 2;
  if(vectorTo.y < -length)
    vectorTo.y += length * 2;
  else if(vectorTo.y > length)
    vectorTo.y -= length * 2;
  return vectorTo;
}

Vector2 Border::getRandomPosInBorder() {
  return (Vector2){(float)(rand() % (int)length), (float)(rand() % (int)length)};
}

float Border::getDistance(Vector2 a, Vector2 b) {
  Vector2 vecTo = getShortestPathToPoint(a, b);
  return Vector2Length(vecTo);
}

Vector2 Border::unwrapPositionRelative(Vector2 alpha, Vector2 beta) {
  Vector2 shortestPath = getShortestPathToPoint(alpha, beta);
  return shortestPath + alpha;
}

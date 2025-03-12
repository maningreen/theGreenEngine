#include "border.hpp"
#include "engine/entity.hpp"
#include "include.h"

#define abs(x) (x < 0 ? -x : x)

float Border::Length = 2000;

Border::Border() : Entity2D("Border", (Vector2){0, 0}){
}

Border::~Border() {}

void Border::Render() {
  DrawRectangleLinesEx((Rectangle){-Length, -Length, Length * 2, Length * 2}, 20, WHITE);
}

void Border::Process(float delta) {
}

void Border::wrapEntity(Entity2D* en) {
  en->Position = wrapPos(en->Position);
}

Vector2 Border::wrapPos(Vector2 p) {
  return (Vector2){wrapPosX(p).x, wrapPosY(p).y};
}

Vector2 Border::wrapPosX(Vector2 v) {
  while(abs(v.x) > Border::Length) {
    if(v.x < -Length)
      v.x += Length * 2;
    else if(v.x > Length)
      v.x -= Length * 2;
  }
  return v;
}

Vector2 Border::wrapPosY(Vector2 v) {
  while(abs(v.y) > Border::Length) {
    if(v.y < -Length)
      v.y += Length * 2;
    else if(v.y > Length)
      v.y -= Length * 2;
  }
  return v;
}

Vector2 Border::getShortestPathToPoint(const Entity2D* en, const Vector2 point) {
  Vector2 vectorTo = Vector2Subtract(point, en->Position);
  if(vectorTo.x < -Length)
    vectorTo.x += Length * 2;
  else if(vectorTo.x > Length)
    vectorTo.x -= Length * 2;
  if(vectorTo.y < -Length)
    vectorTo.y += Length * 2;
  else if(vectorTo.y > Length)
    vectorTo.y -= Length * 2;
  return vectorTo;
}

Vector2 Border::getShortestPathToPoint(const Vector2 a, const Vector2 b) {
  Vector2 vectorTo = Vector2Subtract(b, a);
  if(vectorTo.x < -Length)
    vectorTo.x += Length * 2;
  else if(vectorTo.x > Length)
    vectorTo.x -= Length * 2;
  if(vectorTo.y < -Length)
    vectorTo.y += Length * 2;
  else if(vectorTo.y > Length)
    vectorTo.y -= Length * 2;
  return vectorTo;
}

Vector2 Border::getRandomPosInBorder() {
  return (Vector2){(float)(rand() % (int)Length), (float)(rand() % (int)Length)};
}

float Border::getDistance(Vector2 a, Vector2 b) {
  Vector2 vecTo = getShortestPathToPoint(a, b);
  return Vector2Length(vecTo);
}

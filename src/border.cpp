#include "border.hpp"
#include "engine/entity.hpp"
#include "include.h"

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
  if(en->Position.x < -Length)
    en->Position.x += Length * 2;
  else if(en->Position.x > Length)
    en->Position.x -= Length * 2;
  if(en->Position.y < -Length)
    en->Position.y += Length * 2;
  else if(en->Position.y > Length)
    en->Position.y -= Length * 2;
}

Vector2 Border::getShortestPathToPoint(Entity2D* en, Vector2 point) {
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

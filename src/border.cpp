#include "border.hpp"
#include "entity.hpp"
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

#include "bars.hpp"
#include "engine/entity.hpp"
#include "include.h"
#include <iostream>

float Bar::SmoothingSpeed = 15;

void Bar::Render() {
  if(!ShouldRender)
    return;
  //STEP UNO calculate the portion that's gonna be drawn OH WAIT THATS OUR PROGRESS
  //step dos we split the Dimensions field up to be in portions (1 - t) and t
  Vector2 dimensionsFull = (Vector2){Dimensions.x * (ShrinkY ? 1 : Progress), Dimensions.y * (ShrinkY ? Progress : 1)};
  Vector2 dimensionsNotFull = (Vector2){Dimensions.x - (!ShrinkY ? dimensionsFull.x : 0), Dimensions.y - (ShrinkY ? dimensionsFull.y : 0)};
  //then we calculate our not full offset
  Vector2 notFullOffset = (Vector2){!ShrinkY ? dimensionsFull.x : 0, ShrinkY ? dimensionsFull.y : 0};
  DrawRectangleV(Position, dimensionsFull, Colour);
  DrawRectangleV(Vector2Add(notFullOffset, Position), dimensionsNotFull, EmptyCol);
}

void Bar::Process(float delta) {
  if(Smooth)
    Progress += (TargetProgress - Progress) * SmoothingSpeed * delta;
  else
    Progress = TargetProgress;
}

Bar::Bar(Vector2 pos, Vector2 dimens, Color colour, Color backCol, bool growVert) : Entity2D("Bar", pos),
  Colour(colour), EmptyCol(backCol), Dimensions(dimens), ShrinkY(growVert) {
  Progress = 1;
  TargetProgress = 1;
  Smooth = true;
  ShouldRender = true;
}

Bar::~Bar() {}

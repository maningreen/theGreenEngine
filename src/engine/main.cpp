#include "entity.hpp"
#include "core.h"
#include "../include.h"
#include <vector>
#include <iostream>

#define baseScreenScalar 1000
#define initialScreenDimensions (Vector2){baseScreenScalar * 16/9, baseScreenScalar / (16 / 9)}

void manageChildrenProcess(std::vector<Entity*>* children, float delta) {
  for(int i = 0; i < children->size(); i++) {
    (*children)[i]->Process(delta);
    manageChildrenProcess(&(*children)[i]->Children, delta);
    if(!(*children)[i]->getValid()) {
      (*children)[i]->kill();
      children->erase(children->begin() + i);
      i--;
      continue;
    }
  }
}

void manageChildrenRendering(std::vector<Entity*>* children) {
  for(Entity* child : *children) {
    manageChildrenRendering(&child->Children);
    child->Render();
  }
}

void Init(Entity* root);

void PreRendering(std::vector<Entity*>* entities);

void PostRendering(std::vector<Entity*>* entities);

int main() {
  Entity* Root = new Entity("Root", 0);

  Entity::setRoot(Root);

  SetTargetFPS(60);

  InitWindow(initialScreenDimensions.x, initialScreenDimensions.y, "Game :)");

  Init(Root);

  float delta = 1.0f / 60.0f;
  while(!WindowShouldClose()) {

    manageChildrenProcess(&Root->Children, delta);

    BeginDrawing();

    PreRendering(&Root->Children);

    manageChildrenRendering(&Root->Children);

    PostRendering(&Root->Children);

    EndDrawing();
  }

  Root->kill();

  CloseWindow();
}

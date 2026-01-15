#include "entity.hpp"
#include <csignal>
#include <vector>

extern "C" {
  extern void hs_init(int argc, char** argv);
  extern void hs_exit();
};

#define baseScreenScalar 1000
#define initialScreenDimensions (Vector2){baseScreenScalar * 16/9, baseScreenScalar / (16 / 9)}

void manageChildrenProcess(std::vector<Entity*>* children, float delta) {
  for(int i = 0; i < children->size(); i++) {
    (*children)[i]->process(delta);
    manageChildrenProcess(&(*children)[i]->children, delta);
    if(!(*children)[i]->getValid()) {
      (*children)[i]->kill();
      if(!(*children)[i]->getValid()) { // last chance!
        children->erase(children->begin() + i--);
        continue;
      }
    }
  }
}

void manageChildrenRendering(std::vector<Entity*>* children) {
  for(Entity* child : *children) {
    manageChildrenRendering(&child->children);
    child->render();
  }
}

void Init(Entity* root);

void PreRendering(Entity* root);

void PostRendering(Entity* root);

int main() {
  hs_init(0, 0);
  SetTraceLogLevel(LOG_NONE);
  Entity Root = Entity("Root", nullptr);
  Entity::setRoot(&Root);

  SetTargetFPS(60);

  srand(time(0));

  InitWindow(initialScreenDimensions.x, initialScreenDimensions.y, "Game :)");

  Init(&Root);

  float delta = 1.0f / 60.0f;
  while(!WindowShouldClose() && Root.getValid()) {
    manageChildrenProcess(&Root.children, delta);

    BeginDrawing();

    PreRendering(&Root);

    manageChildrenRendering(&Root.children);

    PostRendering(&Root);

    EndDrawing();
  }

  CloseWindow();

  hs_exit();
}

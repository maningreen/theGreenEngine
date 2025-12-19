#include "entity.hpp"
#include <csignal>
#include <vector>

#define baseScreenScalar 1000
#define initialScreenDimensions (Vector2){baseScreenScalar * 16/9, baseScreenScalar / (16 / 9)}

void manageChildrenProcess(std::vector<Entity*>* children, float delta) {
  for(int i = 0; i < children->size(); i++) {
    (*children)[i]->process(delta);
    manageChildrenProcess(&(*children)[i]->children, delta);
    if(!(*children)[i]->getValid()) {
      (*children)[i]->kill();
      if(!(*children)[i]->getValid()) { // last chance for redemption!
        children->erase(children->begin() + i--);
        continue;
      }
    }
  }
}

void manageChildrenRendering(Entity* en) {
  for(Entity* child : en->children) {
    manageChildrenRendering(child);
    child->render();
  }
}

void Init(Entity* root);

void PreRendering(Entity* root);

void PostRendering(Entity* root);

int main() {
  Entity root = Entity("Root", nullptr);
  Entity::setRoot(&root);

  SetTargetFPS(60);

  InitWindow(initialScreenDimensions.x, initialScreenDimensions.y, "Game :)");

  Init(&root);

  float delta = 1.0f / 60.0f;
  while(!WindowShouldClose() && root.getValid()) {
    manageChildrenProcess(&root.children, delta);

    BeginDrawing();

    PreRendering(&root);

    manageChildrenRendering(&root);

    PostRendering(&root);

    EndDrawing();
  }

  CloseWindow();
}

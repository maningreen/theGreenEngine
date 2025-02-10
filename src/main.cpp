#include "entity.hpp"
#include "player.hpp"
#include "camera.hpp"
#include "core.h"
#include "postprocessing.hpp"
#include <raylib.h>
#include <raymath.h>
#include <vector>

#define baseScreenScalar 1000
#define initialScreenDimensions (Vector2){baseScreenScalar * 16/9, baseScreenScalar / (16 / 9)}

void manageChildrenProcess(std::vector<Entity*>* children, float delta) {
  int i = 0;
  for(Entity* child : *children) {
    manageChildrenProcess(&child->Children, delta);
    child->Process(delta);
    if(!child->valid) {
      delete child;
      (*children)[i] = children->back();
      children->pop_back();
      continue;
    }
    i++;
  }
}

void manageChildrenRendering(std::vector<Entity*>* children) {
  for(Entity* child : *children) {
    manageChildrenRendering(&child->Children);
    child->Render();
  }
}

void Init(std::vector<Entity*>* entities) {
  Engine::addEntity(entities, new PostProcessingData());
  Engine::addEntity(entities, new Player("Player", (Vector2){0, 0}));
  (*entities)[0]->addChild(new CameraEntity("Camera", &((Player*)(*entities)[1])->Position));
}

void PreRendering(std::vector<Entity*>* entities) {
  PostProcessingData* data = (PostProcessingData*)entities->front();
  BeginTextureMode(data->Texture);
}

void PostRendering(std::vector<Entity*>* entities) {
  PostProcessingData* data = (PostProcessingData*)entities->front();
  EndTextureMode();
  BeginShaderMode(data->getShader());
  DrawTextureRec(data->Texture.texture, (Rectangle){0, 0, (float)data->Texture.texture.width, -(float)data->Texture.texture.height}, (Vector2){0, 0}, WHITE);
  EndShaderMode();
}

int main() {
  std::vector<Entity*> entities;

  SetTargetFPS(60);

  InitWindow(initialScreenDimensions.x, initialScreenDimensions.y, "Game engine");

  Init(&entities);

  float delta = 1.0f / 60.0f;
  while(!WindowShouldClose()) {

    manageChildrenProcess(&entities, delta);

    BeginDrawing();

    PreRendering(&entities);

    manageChildrenRendering(&entities);

    PostRendering(&entities);

    EndDrawing();
  }

  CloseWindow();
}

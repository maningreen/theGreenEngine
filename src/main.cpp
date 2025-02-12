#include "entity.hpp"
#include "player.hpp"
#include "camera.hpp"
#include "core.h"
#include "postprocessing.hpp"
#include "bars.hpp"
#include "border.hpp"
#include <raylib.h>
#include <raymath.h>
#include <vector>
#include <iostream>

#define baseScreenScalar 1000
#define initialScreenDimensions (Vector2){baseScreenScalar * 16/9, baseScreenScalar / (16 / 9)}

void manageChildrenProcess(std::vector<Entity*>* children, float delta) {
  int i = 0;
  for(std::vector<Entity*>::iterator it = children->begin(); it != children->end(); it++) {
    (*children)[i]->Process(delta);
    manageChildrenProcess(&(*children)[i]->Children, delta);
    if(!(*children)[i]->valid) {
      Engine::popEntityFromChildren(children, i, it);
      it--;
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
  Engine::addEntity(entities, new Player("Player", (Vector2){0, 0}, nullptr));
  (*entities)[0]->addChild(new CameraEntity("Camera", &((Player*)(*entities)[1])->Position));
  ((Player*)(*entities)[1])->setCam((CameraEntity*)Engine::searchTreeForEntity(entities, "Camera"));
  Engine::addEntity(entities, new Border());
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

  DrawFPS(0, 0);
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

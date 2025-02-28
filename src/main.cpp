#include "entity.hpp"
#include "player.hpp"
#include "camera.hpp"
#include "core.h"
#include "postprocessing.hpp"
#include "bars.hpp"
#include "border.hpp"
#include "enemy.hpp"
#include "spiral.hpp"
#include "include.h"
#include <vector>
#include <iostream>

#define baseScreenScalar 1000
#define initialScreenDimensions (Vector2){baseScreenScalar * 16/9, baseScreenScalar / (16 / 9)}

void manageChildrenProcess(std::vector<Entity*>* children, float delta) {
  int i = 0;
  for(std::vector<Entity*>::iterator it = children->begin(); it != children->end() && !children->empty(); it++) {
    (*children)[i]->Process(delta);
    manageChildrenProcess(&(*children)[i]->Children, delta);
    if(!(*children)[i]->valid) {
      Engine::deInitEntity((*children)[i]);
      children->erase(it);
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

void Init(Entity* root) {
  root->addChild(new PostProcessingData());
  root->addChild(new Player("Player", (Vector2){0, 0}, nullptr));
  root->Children[0]->addChild(new CameraEntity("Camera", &((Player*)(root->Children)[1])->Position));
  ((Player*)(root->Children)[1])->setCam((CameraEntity*)Engine::searchTreeForEntity(&root->Children, "Camera"));
  root->addChild(new Border());
  root->addChild(new Spiraler());
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
  Entity* Root = new Entity("Root", nullptr);

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

  CloseWindow();
}

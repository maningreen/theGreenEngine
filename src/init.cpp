#include "engine/entity.hpp"
#include "engine/core.h"
#include "postprocessing.hpp"
#include "player.hpp"
#include "border.hpp"
#include "dasher.hpp"

void Init(Entity* root) {
  root->addChild(new PostProcessingData());
  root->addChild(new Player("Player", (Vector2){0, 0}, nullptr));
  root->Children[0]->addChild(new CameraEntity("Camera", &((Player*)(root->Children)[1])->Position));
  ((Player*)(root->Children)[1])->setCam((CameraEntity*)Engine::searchTreeForEntity(&root->Children, "Camera"));
  root->addChild(new Border());
  root->addChild(new Dasher((Vector2){600, 600}));
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

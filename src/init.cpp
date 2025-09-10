#include "camera.hpp"
#include "engine/entity.hpp"
#include "engine/core.h"
#include "postprocessing.hpp"
#include "player.hpp"
#include "border.hpp"
#include "spiral.hpp"
#include "time.h"
#include <raylib.h>
#include <raymath.h>

#define shader

void Init(Entity* root) {
  srand(time(0));
  root->addChild(new PostProcessingData());
  Player* plr = new Player("Player", (Vector2){0, 0,}, nullptr);
  root->addChild(plr);
  root->Children[0]->addChild(new CameraEntity("Camera", &plr->Position));
  ((Player*)(root->Children)[1])->setCam((CameraEntity*)Engine::searchTreeForEntity(&root->Children, "Camera"));
  root->addChild(new Border());
  root->addChild(new Enemy({1000, 1000}));
}

void PreRendering(std::vector<Entity*>* entities) { 
  PostProcessingData* data = (PostProcessingData*)entities->front();

  BeginTextureMode(data->Texture);
  BeginMode2D((Camera2D){.offset = Vector2Zero(), .target = {-Border::Length, -Border::Length}, .rotation = 0, .zoom = 1});
}

void PostRendering(std::vector<Entity*>* entities) {
  PostProcessingData* data = (PostProcessingData*)entities->front();
  CameraEntity* cameraEnt = (CameraEntity*)Engine::searchTreeForEntity(entities, "Camera");
  EndTextureMode();

#ifdef shader
  BeginShaderMode(data->getShader());
#endif
  BeginMode2D((Camera2D){.offset = cameraEnt->Camera.offset, .target = Vector2Subtract(cameraEnt->Camera.target, (Vector2){-Border::Length, -Border::Length}), .rotation = 0, .zoom = cameraEnt->Camera.zoom});

  DrawTexturePro(data->Texture.texture, (Rectangle){0, 0, 3 * (float)data->Texture.texture.width, 3 * -(float)data->Texture.texture.height}, (Rectangle){-(float)data->Texture.texture.width, -(float)data->Texture.texture.height, 3 * (float)data->Texture.texture.width, 3 * (float)data->Texture.texture.height}, {0, 0}, 0, WHITE);

  EndMode2D();
#ifdef shader
  EndShaderMode();
#endif

  DrawFPS(0, 0);
}

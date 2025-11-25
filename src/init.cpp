#include "camera.hpp"
#include "enemy.hpp"
#include "engine/entity.hpp"
#include "engine/core.h"
#include "postprocessing.hpp"
#include "player.hpp"
#include "border.hpp"
#include "raylib.h"
#include "inputManager.hpp"
#include "time.h"
#include "waveManager.hpp"

extern "C" {
  extern void hs_init(int argc, char** argv);
};

// #define shader

void Init(Entity* root) {
  hs_init(0, 0);
  srand(time(0));
  root->addChild(new PostProcessingData());
  Player* plr = new Player("Player", (Vector2){0, 0}, nullptr);
  root->addChild(plr);
  root->addChild(new Border());
  root->addChild(new Enemy({200, 200}));
  InputManager* manager = new InputManager();
  // manager->
  root->addChild(manager);
  // root->addChild(new WaveManager());
}

void PreRendering(std::vector<Entity*>* entities) { 
  PostProcessingData* data = (PostProcessingData*)entities->front();

  BeginTextureMode(data->Texture);
  BeginMode2D((Camera2D){.offset = Vector2Zero(), .target = {-Border::length, -Border::length}, .rotation = 0, .zoom = 1});
}

void PostRendering(std::vector<Entity*>* entities) {
  PostProcessingData* data = (PostProcessingData*)entities->front();
  CameraEntity* cameraEnt = (CameraEntity*)Engine::searchTreeForEntity(entities, "Camera");
  EndTextureMode();

#ifdef shader
  BeginShaderMode(data->getShader());
#endif
  BeginMode2D((Camera2D){.offset = cameraEnt->Camera.offset, .target = Vector2Subtract(cameraEnt->Camera.target, (Vector2){-Border::length, -Border::length}), .rotation = 0, .zoom = cameraEnt->Camera.zoom});

  DrawTexturePro(data->Texture.texture, (Rectangle){0, 0, 3 * (float)data->Texture.texture.width, 3 * -(float)data->Texture.texture.height}, (Rectangle){-(float)data->Texture.texture.width, -(float)data->Texture.texture.height, 3 * (float)data->Texture.texture.width, 3 * (float)data->Texture.texture.height}, {0, 0}, 0, WHITE);

  EndMode2D();
#ifdef shader
  EndShaderMode();
#endif

  DrawFPS(0, 0);
}

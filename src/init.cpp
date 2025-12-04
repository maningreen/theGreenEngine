#include "camera.hpp"
#include "dasher.hpp"
#include "enemy.hpp"
#include "engine/entity.hpp"
#include "engine/core.h"
#include "include.h"
#include "postprocessing.hpp"
#include "player.hpp"
#include "border.hpp"
#include "raylib.h"
#include "inputManager.hpp"
#include "sniper.hpp"
#include "spiral.hpp"
#include "console.hpp"
#include "time.h"

extern "C" {
  extern void hs_init(int argc, char** argv);
};

// #define shader

Console* console;

void Init(Entity* root) {
  hs_init(0, 0);
  srand(time(0));
  root->addChild(new PostProcessingData());
  console = new Console;
  root->addChild(console);

  Player* plr = new Player("Player", (Vector2){0, 0});
  root->addChild(plr);
  root->addChild(new Border());
  root->addChild(new Enemy({200, 200}));
  root->addChild(new Enemy({1200, 200}));
  // root->addChild(new Dasher({200, 200}));
  // root->addChild(new Spiraler({200, 200}));
  // root->addChild(new Sniper({200, 200}));
  // InputManager* manager = new InputManager();
  // root->addChild(manager);
}

void PreRendering(std::vector<Entity*>* entities) { 
  PostProcessingData* data = (PostProcessingData*)entities->front();

  BeginTextureMode(data->texture);
  BeginMode2D((Camera2D){
    .offset = Vector2Zero(),
    .target = {
      -Border::length,
      -Border::length
    },
    .rotation = 0,
    .zoom = 1
  });
}

void PostRendering(std::vector<Entity*>* entities) {
  PostProcessingData* data = (PostProcessingData*)entities->front();
  static CameraEntity* cameraEnt;
  if(cameraEnt == nullptr)
    cameraEnt = (CameraEntity*)Engine::searchTreeForEntity(entities, "Camera");
  EndTextureMode();

#ifdef shader
  BeginShaderMode(data->getShader());
#endif
  BeginMode2D(
    (Camera2D){
      .offset = cameraEnt->Camera.offset, 
      .target = Vector2Subtract(cameraEnt->Camera.target, (Vector2){-Border::length, -Border::length}), 
      .rotation = 0, .zoom = cameraEnt->Camera.zoom
    }
  );

  DrawTexturePro(data->texture.texture, (Rectangle){0, 0, 3 * (float)data->texture.texture.width, 3 * -(float)data->texture.texture.height}, (Rectangle){-(float)data->texture.texture.width, -(float)data->texture.texture.height, 3 * (float)data->texture.texture.width, 3 * (float)data->texture.texture.height}, {0, 0}, 0, WHITE);
  EndMode2D();
#ifdef shader
  EndShaderMode();
#endif

  DrawFPS(0, 0);
  console->postProcessingRender();
}

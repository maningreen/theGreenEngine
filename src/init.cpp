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

#define shader

Console* console = nullptr;
CameraEntity* cameraEnt = nullptr;
PostProcessingData* data = nullptr;

void Init(Entity* root) {
  DEBUG;
  console = new Console;
  DEBUG;
  hs_init(0, 0);
  DEBUG;
  srand(time(0));
  DEBUG;
  data = new PostProcessingData();
  DEBUG;
  root->addChild(data);
  DEBUG;
  root->addChild((Entity*)console);
  DEBUG;

  DEBUG;
  Player* plr = new Player("Player", (Vector2){0, 0});
  DEBUG;
  cameraEnt = plr->getCam();
  DEBUG;
  root->addChild((Entity*)plr);
  DEBUG;
  root->addChild((Entity*)new Border());
  DEBUG;
  root->addChild((Entity*)new Enemy({200, 200}));
  DEBUG;
  root->addChild((Entity*)new Enemy({1200, 200}));
  DEBUG;
  // root->addChild(new Dasher({200, 200}));
  // root->addChild(new Spiraler({200, 200}));
  // root->addChild(new Sniper({200, 200}));
}

void PreRendering(Entity* root) { 
  DEBUG;
  BeginTextureMode(data->texture);
  DEBUG;
  BeginMode2D((Camera2D){
    .offset = Vector2Zero(),
    .target = {
      -Border::length,
      -Border::length
    },
    .rotation = 0,
    .zoom = 1
  });
  DEBUG;
}

void PostRendering(Entity* root) {
  DEBUG;
  EndTextureMode();
  DEBUG;

#ifdef shader
  DEBUG;
  BeginShaderMode(data->getShader());
  DEBUG;
#endif
  DEBUG;
  BeginMode2D(
    (Camera2D){
      .offset = cameraEnt->Camera.offset, 
      .target = Vector2Subtract(cameraEnt->Camera.target, (Vector2){-Border::length, -Border::length}), 
      .rotation = 0, .zoom = cameraEnt->Camera.zoom
    }
  );
  DEBUG;

  int countX = ceilf((float)GetScreenWidth() / Border::length) * 2 + 1;
  int countY = ceilf((float)GetScreenHeight() / Border::length) * 2 + 1;
  DEBUG;
  DrawTexturePro(
    data->texture.texture,
    (Rectangle){
      0,
      0,
      countX * (float)data->texture.texture.width,
      countY * -(float)data->texture.texture.height
    },
    (Rectangle){
      -((countX) / 2) * (float)data->texture.texture.width,
      -((countY) / 2) * (float)data->texture.texture.height,
      countX * (float)data->texture.texture.width,
      countY * (float)data->texture.texture.height
    },
    (Vector2){
      0,
      0
    },
    0,
    WHITE
  );
  DEBUG;
  EndMode2D();
  DEBUG;
#ifdef shader
  DEBUG;
  EndShaderMode();
  DEBUG;
#endif

  DrawFPS(0, 0);
  DEBUG;
  console->postProcessingRender();
  DEBUG;
}

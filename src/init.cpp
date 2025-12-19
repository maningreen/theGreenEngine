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
  console = new Console;
  hs_init(0, 0);
  srand(time(0));
  data = new PostProcessingData();
  root->addChild(data);
  root->addChild((Entity*)console);

  Player* plr = new Player("Player", (Vector2){0, 0});
  cameraEnt = plr->getCam();
  root->addChild((Entity*)plr);
  root->addChild((Entity*)new Border());
  root->addChild((Entity*)new Enemy({200, 200}));
  root->addChild((Entity*)new Enemy({1200, 200}));
  // root->addChild(new Dasher({200, 200}));
  // root->addChild(new Spiraler({200, 200}));
  // root->addChild(new Sniper({200, 200}));
}

void PreRendering(Entity* root) { 
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

void PostRendering(Entity* root) {
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

  int countX = ceilf((float)GetScreenWidth() / Border::length) * 2 + 1;
  int countY = ceilf((float)GetScreenHeight() / Border::length) * 2 + 1;
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
  EndMode2D();
#ifdef shader
  EndShaderMode();
#endif

  DrawFPS(0, 0);
  console->postProcessingRender();
}

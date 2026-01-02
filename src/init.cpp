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
#include "cursor.hpp"
#include "spiral.hpp"
#include "time.h"

extern "C" {
  extern void hs_init(int argc, char** argv);
};

CameraEntity* cameraEnt;
PostProcessingData* data;

#define shader shading

void Init(Entity* root) {
  hs_init(0, 0);
  srand(time(0));
  data = new PostProcessingData();
  root->addChild(data);
  root->addChild(new Cursor());

  Player* plr = new Player("Player", (Vector2){0, 0});
  root->addChild(plr);
  root->addChild(new Border());
  root->addChild(new Enemy({200, 200}));
  root->addChild(new Enemy({1200, 200}));
  cameraEnt = plr->getCamera();
  // root->addChild(new Dasher({200, 200}));
  // root->addChild(new Spiraler({200, 200}));
  // root->addChild(new Sniper({200, 200}));
  // InputManager* manager = new InputManager();
  // root->addChild(manager);
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
  PostProcessingData* data = (PostProcessingData*)root->children.front();
  if(cameraEnt == nullptr)
    cameraEnt = (CameraEntity*)Engine::searchTreeForEntity(&root->children, "Camera");
  EndTextureMode();

#ifdef shader
  BeginShaderMode(data->getShader());
#endif
  BeginMode2D(
    (Camera2D){
      .offset = cameraEnt->camera.offset, 
      .target = Vector2Subtract(cameraEnt->camera.target, (Vector2){-Border::length, -Border::length}), 
      .rotation = 0, .zoom = cameraEnt->camera.zoom
    }
  );

  unsigned int renderXCount = 2 * ceil((int)GetScreenWidth() / Border::length) + 1;
  unsigned int renderYCount = 2 * ceil((int)GetScreenHeight() / Border::length) + 1;
  DrawTexturePro(
    data->texture.texture,
    (Rectangle){
      0,
      0,
      renderXCount * (float)data->texture.texture.width,
      renderYCount * -(float)data->texture.texture.height
    }, 
    (Rectangle){
      -((int)(renderXCount / 2)) * (float)data->texture.texture.width,
      -((int)(renderYCount / 2)) * (float)data->texture.texture.height,
      renderXCount * (float)data->texture.texture.width,
      renderYCount * (float)data->texture.texture.height
    }, 
    {
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
}

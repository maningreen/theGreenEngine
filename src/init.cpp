#include "camera.hpp"
#include "dasher.hpp"
#include "enemy.hpp"
#include "engine/entity.hpp"
#include "engine/core.h"
#include "include.h"
#include "postProcessing.hpp"
#include "player.hpp"
#include "border.hpp"
#include "raylib.h"
#include "inputManager.hpp"
#include "sniper.hpp"
#include "cursor.hpp"
#include "spiral.hpp"
#include "time.h"

#include "store.hpp"

extern "C" {
  extern void hs_init(int argc, char** argv);
};

CameraEntity* cameraEnt;
PostProcessing* data;

void managePostRendering(Entity* en) {
  for(Entity* kid : en->children)
    managePostRendering(kid);
  en->postProcessingRender();
}

void Init(Entity* root) {
  hs_init(0, 0);
  srand(time(0));

  root->addChild(new Cursor());
  root->addChild(new Store());
  data = new PostProcessing();
  root->addChild(data);

  Player* plr = new Player("Player", (Vector2){0, 0});
  root->addChild(plr);
  root->addChild(new Border());
  root->addChild(new Enemy({200, 200}));
  root->addChild(new Enemy({1200, 200}));
  // root->addChild(new Dasher({200, 200}));
  // root->addChild(new Spiraler({200, 200}));
  // root->addChild(new Sniper({200, 200}));
}

void PreRendering(Entity* root) { 
  data->preRender();
}

void PostRendering(Entity* root) {
  data->postRender();

  BeginMode2D(Player::player->getCamera().camera);

  managePostRendering(root);

  EndMode2D();

  DrawFPS(0, 0);

  data->postPostRender();
}

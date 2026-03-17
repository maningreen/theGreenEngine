#include "border.hpp"
#include "button.hpp"
#include "camera.hpp"
#include "cursor.hpp"
#include "enemy.hpp"
#include "engine/entity.hpp"
#include "engine/world.hpp"
#include "include.h"
#include "inputManager.hpp"
#include "player.hpp"
#include "postProcessing.hpp"
#include "raylib.h"
#include "sniper.hpp"
#include "store.hpp"
#include "time.h"
#include "wave.hpp"

CameraEntity* cameraEnt = nullptr;
PostProcessing* data = nullptr;

void managePostRendering() {
    for(Entity* en : World::get().entities) en->postProcessingRender();
}

void init() {
    ModManager::get()->initLua();
    World::addEntity(new Button(Vector2Zero(), "Begin Game", true, [](Button* self) {
        World::addEntity(new WaveManager);
    }));

    World::addEntity(&Player::get());

    World::addEntity(new Cursor());
    data = new PostProcessing();
    World::addEntity(data);
    World::addEntity(new Border());
    // World::addEntity(new Enemy({200, 200}));
    // World::addEntity(new Enemy({1200, 200}));
    // World::addEntity(new Dasher({200, 200}));
    // World::addEntity(new Spiraler({200, 200}));
    // World::addEntity(new Sniper({200, 200}));
}

void preRender() {
    data->preRender();
}

void postRender() {
    data->postRender();

    BeginMode2D(Player::get().getCamera().camera);

    managePostRendering();

    EndMode2D();

    DrawFPS(0, 0);

    data->postPostRender();
}

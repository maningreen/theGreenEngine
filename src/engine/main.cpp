#include <raylib.h>

#include <iostream>
#include <vector>

#include "entity.hpp"
#include "world.hpp"

#define baseScreenScalar 1000
#define initialScreenDimensions                                \
    (Vector2) {                                                \
        baseScreenScalar * 16 / 9, baseScreenScalar / (16 / 9) \
    }

extern "C" {
double getDelta();
}

void init();

void postRender();

void preRender();

int main() {
    World::init();

    srand(time(0));
    SetTraceLogLevel(LOG_ERROR);

    SetTargetFPS(60);

    InitWindow(initialScreenDimensions.x, initialScreenDimensions.y, "Game :)");

    while(!IsWindowReady()) {
    }

    init();

    while(!WindowShouldClose()) {
        float delta = getDelta();
        World::process(delta);

        BeginDrawing();

        preRender();

        World::render();

        postRender();

        EndDrawing();
    }

    CloseWindow();

    World::deinit();

    return 0;
}

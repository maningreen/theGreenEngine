#include <raylib.h>

#include <iostream>
#include <vector>

#include "entity.hpp"
#include "world.hpp"

#define baseScreenScalar 1
#define initialScreenDimensions                                 \
    (Vector2) {                                                 \
        baseScreenScalar * 1920, baseScreenScalar * 1080, \
    }

extern "C" {
double getDelta();
}

extern void init();

extern void postRender();

extern void preRender();

int main() {
    World::init();

    srand(time(0));
    SetTraceLogLevel(LOG_ERROR);

    SetTargetFPS(120);

    InitWindow(initialScreenDimensions.x, initialScreenDimensions.y, "Game :)");

    while(!IsWindowReady());

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

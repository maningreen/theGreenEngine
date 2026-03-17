#include <raylib.h>

#include <vector>

#include "entity.hpp"
#include "world.hpp"

#define baseScreenScalar 1000
#define initialScreenDimensions                                \
    (Vector2) {                                                \
        baseScreenScalar * 16 / 9, baseScreenScalar / (16 / 9) \
    }

void init();

void postRender();

void preRender();

int main() {
    srand(time(0));
    SetTraceLogLevel(LOG_ALL);

    SetTargetFPS(60);

    InitWindow(initialScreenDimensions.x, initialScreenDimensions.y, "Game :)");

    while(!IsWindowReady()) { }

    init();

    float delta = 1.0f / 60.0f;
    while(!WindowShouldClose()) {
        World::process(delta);

        BeginDrawing();

        preRender();

        World::render();

        postRender();

        EndDrawing();
    }

    CloseWindow();

    return 0;
}

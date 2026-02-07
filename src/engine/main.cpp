#include <csignal>
#include <vector>

#include "world.hpp"
#include "entity.hpp"

extern "C" {
extern void hs_init(int argc, char** argv);
extern void hs_exit();
};

#define baseScreenScalar 1000
#define initialScreenDimensions                                \
    (Vector2) {                                                \
        baseScreenScalar * 16 / 9, baseScreenScalar / (16 / 9) \
    }

void Init();

void PreRendering();

void PostRendering();

int main() {
    srand(time(0));
    hs_init(0, 0);
    SetTraceLogLevel(LOG_NONE);

    SetTargetFPS(60);

    srand(time(0));

    InitWindow(initialScreenDimensions.x, initialScreenDimensions.y, "Game :)");

    Init();

    float delta = 1.0f / 60.0f;
    while(!WindowShouldClose()) {
        World::process(delta);

        BeginDrawing();

        PreRendering();

        World::render();

        PostRendering();

        EndDrawing();
    }

    CloseWindow();

    hs_exit();
}

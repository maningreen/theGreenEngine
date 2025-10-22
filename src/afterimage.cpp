#include "afterimage.hpp"
#include "engine/entity.hpp"
#include "raylib.h"

float Afterimage::fadePower = 5;

Afterimage::Afterimage(Vector2 pos, float angle) : Entity2D("Afterimage", pos), rotation(angle) {
  opacity = 255;
}

Afterimage::~Afterimage() {}

void Afterimage::Render() {
  const float distance = 50;
  DrawTriangle(Position,
    Position + (Vector2){cosf(rotation) * distance, sinf(rotation) * distance},
    Position + (Vector2){cosf(rotation + 4 * PI / 3) * distance,
                 sinf(rotation + 4 * PI / 3) * distance},
    (Color){YELLOW.r, YELLOW.g, YELLOW.b, (unsigned char)opacity});
  DrawTriangle(Position,
    Position + (Vector2){cosf(rotation + 2 * PI / 3) * distance,
                 sinf(rotation + 2 * PI / 3) * distance},
    Position + (Vector2){cosf(rotation) * distance, sinf(rotation) * distance},
    (Color){YELLOW.r, YELLOW.g, YELLOW.b, (unsigned char)opacity});
}

void Afterimage::Process(float delta) {
  opacity += -opacity * fadePower * delta;
  if(opacity < 10)
    killDefered();
}

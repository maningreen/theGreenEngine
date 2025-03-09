#include "afterimage.hpp"
#include "engine/entity.hpp"
#include "include.h"

float Afterimage::FadePower = 5;

Afterimage::Afterimage(Vector2 pos, float angle) : Entity2D("Afterimage", pos), Rotation(angle) {
  Opacity = 255;
}

Afterimage::~Afterimage() {}

void Afterimage::Render() {
  const float distance = 50;
  DrawTriangle(
      Position,
      Vector2Add(Position, (Vector2){cosf(Rotation + 4 * M_PI / 3) * distance, -sinf(Rotation + 4 * M_PI / 3) * distance}),
      Vector2Add(Position, (Vector2){cosf(Rotation) * distance, -sinf(Rotation) * distance}),
        (Color){YELLOW.r, YELLOW.g, YELLOW.b, (unsigned char)Opacity}
      );
  DrawTriangle(
      Position,
      Vector2Add(Position, (Vector2){cosf(Rotation) * distance, -sinf(Rotation) * distance}),
      Vector2Add(Position, (Vector2){cosf(Rotation + 2 * M_PI / 3) * distance, -sinf(Rotation + 2 * M_PI / 3) * distance}),
        (Color){YELLOW.r, YELLOW.g, YELLOW.b, (unsigned char)Opacity}
      );

}

void Afterimage::Process(float delta) {
  Opacity += -Opacity * FadePower * delta;
  if(Opacity < 10)
    killDefered();
}

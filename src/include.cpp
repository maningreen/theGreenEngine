#include "include.h"

Vector2 getInputVector(int u, int d, int l, int r) {
  Vector2 out = (Vector2){(float)IsKeyDown(r) - (float)IsKeyDown(l),
                          (float)IsKeyDown(d) - (float)IsKeyDown(u)};
  return Vector2Normalize(out);
}

#include "include.h"

Vector2 getInputVector(int u, int d, int l, int r) {
  Vector2 out = (Vector2) {(float)IsKeyDown(r) - (float)IsKeyDown(l),
      (float)IsKeyDown(d) - (float)IsKeyDown(u)};
  return Vector2Normalize(out);
}

int getQuadrantOfVector(const Vector2 x) {
  // kind hard to read, sorry... use indentation just pretend it's python or something
  if(x.x > 0)
    if(x.y > 0)
      return 1;
    else
      return 4;
  else if(x.y > 0)
    return 2;
  else
    return 3;
}

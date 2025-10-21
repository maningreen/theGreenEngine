#include "include.h"
#include "border.hpp"

Vector2 getInputVector(int u, int d, int l, int r) {
  Vector2 out = (Vector2) {(float)IsKeyDown(r) - (float)IsKeyDown(l),
      (float)IsKeyDown(d) - (float)IsKeyDown(u)};
  return Vector2Normalize(out);
}

// calculates the area of a triangle with three *unwrapped* coordinates
float calculateTriangleArea(Vector2 a, Vector2 b, Vector2 c) {

  // formula for area of a triangle b * h / 2
  float bLength = Border::getDistance(a, b);

  float theta =
      atan2(b.y - a.y, b.x - a.x); // getting theta for the operation...
  Vector2 shortestVector = c - a;

  // we apply a transformation matrix, but only keep the y, as that's the
  // relative height
  float triangleHeight =
    -shortestVector.x * sin(theta) + shortestVector.y * cos(theta);

  return abs(bLength * triangleHeight / 2);
}

bool getTriangleIsValid(float angleSum) { return angleSum - PI < DEG2RAD; }

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

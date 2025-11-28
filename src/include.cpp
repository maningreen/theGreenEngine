#include "include.h"
#include "border.hpp"

#define min(a, b) (a < b ? a : b)

Vector2 getInputVector(Key u, Key d, Key l, Key r) {
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

bool calculateCollisionTriangleCircle(Vector2 a, Vector2 b, Vector2 c, Vector2 center, float radius) {
  Vector2 avg = Vector2Scale(a + b + c, 1.0f / 3.0f);

  Vector2 vecToAvg = avg - center;
  float dist = Vector2Length(vecToAvg);

  float min = min(dist, radius);
  Vector2 p = center +  Vector2Scale(vecToAvg, min / dist);

  return CheckCollisionPointTriangle(p, a, b, c);
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

bool CheckCollisionCircleRecEx(Vector2 center, float radius, Vector2 rectPos, Vector2 dimensions, float angle) {
  // ...
  // oh god here we go
  // step one figure out the center
  Vector2 diff = Vector2Subtract(center, rectPos);
  Vector2 angleVec = (Vector2){cosf(-angle), sinf(-angle)};
  // then we transform the circle to local space
  Vector2 localPos = (Vector2){diff.x * angleVec.x - diff.y * angleVec.y, diff.x * angleVec.y + diff.y * angleVec.x};
  Vector2 dems = Vector2Scale(dimensions, .5f);
  Vector2 finalPos = (Vector2){
    fmaxf(-dems.x, fminf(localPos.x, dems.x)),
    fmaxf(-dems.y, fminf(localPos.y, dems.y))
  };
  return Vector2Distance(finalPos, localPos) <= radius;
}


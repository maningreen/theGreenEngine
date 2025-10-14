#ifndef in_h
#define in_h

#include <raylib.h>
#include <raymath.h>

Vector2 getInputVector(int u, int d, int l, int r);
int getQuadrantOfVector(const Vector2 x);

#endif

#ifndef in_h
#define in_h

#include <raylib.h>
#include <raymath.h>

Vector2 getInputVector(int u, int d, int l, int r);
int getQuadrantOfVector(const Vector2 x);

#define vecsToRec(a, b) (Rectangle){a.x, a.y, b.x, b.y}
#define printDebug() printf("Passed Line: %d, In File: %s\n", __LINE__, __FILE__)

#endif

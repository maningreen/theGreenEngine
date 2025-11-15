#ifndef in_h
#define in_h

#include <raylib.h>
#include <raymath.h>

typedef int Key;

Vector2 getInputVector(Key u, Key d, Key l, Key r);
int getQuadrantOfVector(const Vector2 x);

// calculates the area of a triangle with three *unwrapped* coordinates
float calculateTriangleArea(Vector2 a, Vector2 b, Vector2 c);

// the law which states that the sum a triangle's angles will be 180 degrees 
// (pi radians)
bool getTriangleIsValid(float angleSum);

#define vecsToRec(a, b) (Rectangle){a.x, a.y, b.x, b.y}

#define DEBUG printf("Passed line: %d, in file: %s, in function %s\n", __LINE__, __FILE__, __FUNCTION__)

#endif

#ifndef in_h
#define in_h

#include <raylib.h>
#include <raymath.h>

Vector2 getInputVector(int u, int d, int l, int r);
int getQuadrantOfVector(const Vector2 x);

// calculates the area of a triangle with three *unwrapped* coordinates
float calculateTriangleArea(Vector2 a, Vector2 b, Vector2 c);

// the law which states that the sum a triangle's angles will be 180 degrees 
// (pi radians)
bool getTriangleIsValid(float angleSum);


#define vecsToRec(a, b) (Rectangle){a.x, a.y, b.x, b.y}
#define printDebug() printf("Passed Line: %d, In File: %s\n", __LINE__, __FILE__)

#endif

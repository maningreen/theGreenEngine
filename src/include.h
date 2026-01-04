#ifndef in_h
#define in_h

#include <raylib.h>
#include <raymath.h>
#include <filesystem>

namespace fs = std::filesystem;

typedef int Key;

Vector2 getInputVector(Key u, Key d, Key l, Key r);
int getQuadrantOfVector(const Vector2 x);

// calculates the area of a triangle with three *unwrapped* coordinates
float calculateTriangleArea(Vector2 a, Vector2 b, Vector2 c);

// the law which states that the sum a triangle's angles will be 180 degrees 
// (pi radians)
bool getTriangleIsValid(float angleSum);

bool CheckCollisionCircleRecEx(Vector2 center, float radius, Vector2 rectPos, Vector2 dimensions, float angle);

// returns a random float from 0 to 1
float randomNormalized1();
// returns a random float from -1 to 1
float randomNormalized2();
// returns a random normalized vector.
Vector2 randomVector();

#define DEBUG printf("Passed line: %d, in file: %s, in function %s\n", __LINE__, __FILE__, __FUNCTION__)

#endif

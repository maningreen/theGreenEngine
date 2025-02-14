#include "asteroid.hpp"
#include "entity.hpp"
#include <cstdlib>
#include <raylib.h>
#include <raymath.h>

Asteroid::Asteroid() : Entity2D("Asteroid", (Vector2){0, 0}) {
  vertexCount = 4;
  Vertexes = (Vector2*)malloc(sizeof(Vector2) * vertexCount);
  Vertexes[0] = {100, 100};
  Vertexes[1] = {100, -100};
  Vertexes[2] = {-100, -100};
  Vertexes[3] = {-100, 100};
}

Asteroid::~Asteroid() {
  free(Vertexes);
}

void Asteroid::Process(float delta) {}

void Asteroid::Render() {
  for(unsigned i = 1; i < vertexCount; i++) {
    unsigned nextV = (i + 1) % vertexCount;
    DrawTriangle(Vertexes[i - 1], Vertexes[i], Vertexes[nextV], BROWN);
  }
}

void Asteroid::CutFromCenter(float angle) {
  //hmmmmmm how to go about this....
  //lets start with a rectangle
  //so, we have a rectangle, say we cut it in half horizontally
  //---------
  //which half do we take?
  //a bunch of other questions similar to that pop up, the answer being
  ///shrug
}

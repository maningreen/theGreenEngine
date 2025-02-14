#ifndef stroid_h
#define stroid_h thisIsForFun

#include "entity.hpp"
#include <raylib.h>

class Asteroid : public Entity2D {
  unsigned vertexCount;
  public:
    Vector2 Velocity;
    Vector2* Vertexes;

    void CutFromCenter(float angle);

    virtual void Process(float delta);
    virtual void Render();

    unsigned GetVertexCount();

    Asteroid();
    ~Asteroid();
};

#endif

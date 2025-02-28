#ifndef after_h
#define after_h

#include "entity.hpp"
#include "include.h"

class Afterimage : public Entity2D {
  public:
    float Rotation;
    float Opacity;

    static float FadePower;

    virtual void Render();
    virtual void Process(float delta);

    Afterimage(Vector2 pos, float angle);
    ~Afterimage();
};

#endif

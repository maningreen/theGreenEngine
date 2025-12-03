#ifndef after_h
#define after_h

#include "engine/entity.hpp"

class Afterimage : public Entity2D {
  public:
    float rotation;
    float opacity;

    static float fadePower;

    virtual void render();
    virtual void process(float delta);

    Afterimage(Vector2 pos, float angle);
    ~Afterimage();
};

#endif

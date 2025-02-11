#ifndef border_h
#define border_h

#include "entity.hpp"

class Border : public Entity2D {
  public:
    static float Length;

    virtual void Render();
    virtual void Process(float delta);

    Border();
    ~Border();
};

#endif

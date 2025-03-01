#ifndef bars_h
#define bars_h

#include "engine/entity.hpp"
#include "include.h"

class Bar : public Entity2D {
  public:
    float Progress;
    float TargetProgress;
    Color Colour;
    Color EmptyCol;
    bool Smooth;
    bool ShouldRender;
    Vector2 Dimensions;
    bool ShrinkY;

    virtual void Render();
    virtual void Process(float delta);

    ~Bar();

    Bar(Vector2 position, Vector2 dimensions, Color Colour, Color EmptyCol, bool growVert);

    static float SmoothingSpeed;
};

#endif

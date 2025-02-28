#ifndef post_h
#define post_h

#include "entity.hpp"
#include "include.h"

class PostProcessingData : public Entity {
  private:
    Shader pixelShader;
    int pixelShaderScreenDimensionsLoc;

  public:
    RenderTexture2D Texture;
    Vector2 ScreenDems;

    virtual void Render();
    virtual void Process(float data);

    Shader getShader();

    PostProcessingData();
    ~PostProcessingData();

};

#endif

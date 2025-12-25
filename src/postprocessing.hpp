#ifndef post_h
#define post_h

#include "engine/entity.hpp"
#include "include.h"

class PostProcessingData : public Entity {
  private:
    Shader pixelShader;
    int pixelShaderScreenDimensionsLoc;

  public:
    RenderTexture2D texture;
    Vector2 screenDems;

    virtual void render();
    virtual void process(float data);

    Shader getShader();

    PostProcessingData();
    ~PostProcessingData();

};

#endif

#ifndef post_h
#define post_h

#include "engine/entity.hpp"
#include "include.h"

class PostProcessingData : public Entity {
  private:
    Shader pixelShader;
    int lenLoc;

  public:
    RenderTexture2D texture;
    Vector2 screenDems;

    void render() override;
    void process(float data) override;

    Shader getShader();

    PostProcessingData();
    ~PostProcessingData();
};

#endif

#ifndef POST_H
#define POST_H

#include "engine/entity.hpp"

class PostProcessing : public Entity {
private:
  Shader paletteShader;
public:
  void process(float delta) override;

  void preRender();
  void postRender();
  void postPostRender();

  float pixelLength;
  RenderTexture texture;

  PostProcessing();
  ~PostProcessing();
};

#endif

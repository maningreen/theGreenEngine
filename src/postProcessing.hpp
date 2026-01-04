#ifndef POST_H
#define POST_H

#include "engine/entity.hpp"

class PostProcessing : public Entity {
public:
  void process(float delta) override;

  void preRender();
  void postRender();

  float pixelLength;
  RenderTexture texture;

  PostProcessing();
  ~PostProcessing();
};

#endif

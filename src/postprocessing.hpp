#ifndef post_h
#define post_h

#include "engine/entity.hpp"
#include "include.h"

class PostProcessingData : public Entity {
private:
  Shader pixelShader;
  int lengthLoc;

public:
  RenderTexture2D texture;

  virtual void process(float data);

  Shader getShader();

  PostProcessingData();
  ~PostProcessingData();
};

#endif

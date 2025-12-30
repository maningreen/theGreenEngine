#include "postprocessing.hpp"
#include "camera.hpp"
#include "engine/entity.hpp"
#include "border.hpp"
#include "include.h"

PostProcessingData::PostProcessingData() : Entity("Data") {
  texture = LoadRenderTexture(2 * Border::length, 2 * Border::length);
  pixelShader = LoadShader(0, "resources/shaders/pixel.glsl");
  lengthLoc = GetShaderLocation(pixelShader, "length");
  SetShaderValue(pixelShader, lengthLoc, &Border::length, SHADER_UNIFORM_FLOAT);
  addTag("PostProcessingData");
}

void PostProcessingData::process(float delta) {
  if(texture.texture.width != Border::length * 2) { 
    UnloadRenderTexture(texture);
    texture = LoadRenderTexture(2 * Border::length, 2 * Border::length);
    SetShaderValue(pixelShader, lengthLoc, &Border::length, SHADER_UNIFORM_FLOAT);
  }
}

PostProcessingData::~PostProcessingData() {
  UnloadRenderTexture(texture);
  UnloadShader(pixelShader);
}

Shader PostProcessingData::getShader() {
  return pixelShader;
}

#include "postprocessing.hpp"
#include "camera.hpp"
#include "engine/entity.hpp"
#include "border.hpp"
#include "include.h"

PostProcessingData::PostProcessingData() : Entity("Data") {
  texture = LoadRenderTexture(2 * Border::length, 2 * Border::length);
  pixelShader = LoadShader(0, "resources/shaders/pixel.glsl");
  lenLoc = GetShaderLocation(pixelShader, "leng");
  SetShaderValue(pixelShader, lenLoc, &Border::length, SHADER_UNIFORM_FLOAT);
  addTag("PostProcessingData");
}

void PostProcessingData::process(float delta) {
  if(Border::length * 2 != texture.texture.width) {
    UnloadRenderTexture(texture);
    texture = LoadRenderTexture(2 * Border::length, 2 * Border::length);
    SetShaderValue(pixelShader, lenLoc, &Border::length, SHADER_UNIFORM_FLOAT);
  }
}

void PostProcessingData::render() {}

PostProcessingData::~PostProcessingData() {
  UnloadRenderTexture(texture);
  UnloadShader(pixelShader);
}

Shader PostProcessingData::getShader() {
  return pixelShader;
}

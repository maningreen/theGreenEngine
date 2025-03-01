#include "postprocessing.hpp"
#include "engine/entity.hpp"
#include "include.h"

PostProcessingData::PostProcessingData() : Entity("Data") {
  ScreenDems = (Vector2){(float)GetScreenWidth(), (float)GetScreenHeight()};
  Texture = LoadRenderTexture(ScreenDems.x, ScreenDems.y);
  pixelShader = LoadShader(0, "resources/shaders/pixel.glsl");
  pixelShaderScreenDimensionsLoc = GetShaderLocation(pixelShader, "screenDems");
  SetShaderValue(pixelShader, pixelShaderScreenDimensionsLoc, &ScreenDems, SHADER_UNIFORM_VEC2);
  addTag("PostProcessingData");
}

void PostProcessingData::Process(float delta) {
  Vector2 currentDems = {(float)GetScreenWidth(), (float)GetScreenHeight()};
  if(*(long*)&currentDems != *(long*)&ScreenDems) { //fancy way to not do an ||
    ScreenDems = currentDems;
    UnloadRenderTexture(Texture);
    Texture = LoadRenderTexture(ScreenDems.x, ScreenDems.y);
    SetShaderValue(pixelShader, pixelShaderScreenDimensionsLoc, &ScreenDems, SHADER_UNIFORM_VEC2);
  }
}

void PostProcessingData::Render() {
}

PostProcessingData::~PostProcessingData() {
  UnloadRenderTexture(Texture);
  UnloadShader(pixelShader);
}

Shader PostProcessingData::getShader() {
  return pixelShader;
}

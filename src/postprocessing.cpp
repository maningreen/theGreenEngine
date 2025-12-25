#include "postprocessing.hpp"
#include "camera.hpp"
#include "engine/entity.hpp"
#include "border.hpp"
#include "include.h"

PostProcessingData::PostProcessingData() : Entity("Data") {
  screenDems = (Vector2){(float)GetScreenWidth(), (float)GetScreenHeight()};
  texture = LoadRenderTexture(2 * Border::length, 2 * Border::length);
  pixelShader = LoadShader(0, "resources/shaders/pixel.glsl");
  pixelShaderScreenDimensionsLoc = GetShaderLocation(pixelShader, "screenDems");
  SetShaderValue(pixelShader, pixelShaderScreenDimensionsLoc, &screenDems, SHADER_UNIFORM_VEC2);
  addTag("PostProcessingData");
}

void PostProcessingData::process(float delta) {
  // Vector2 currentDems = {(float)GetScreenWidth(), (float)GetScreenHeight()};
  // if(*(long*)&currentDems != *(long*)&ScreenDems) { //fancy way to not do an ||
    // ScreenDems = currentDems;
    // UnloadRenderTexture(Texture);
    // Texture = LoadRenderTexture(ScreenDems.x, ScreenDems.y);
    // SetShaderValue(pixelShader, pixelShaderScreenDimensionsLoc, &ScreenDems, SHADER_UNIFORM_VEC2);
  // }
}

void PostProcessingData::render() {}

PostProcessingData::~PostProcessingData() {
  UnloadRenderTexture(texture);
  UnloadShader(pixelShader);
}

Shader PostProcessingData::getShader() {
  return pixelShader;
}

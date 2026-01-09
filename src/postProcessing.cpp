#include "postProcessing.hpp"
#include "border.hpp"
#include "player.hpp"

const std::string PostProcessing::palletPath = "resources/shaders/palette.glsl";

#define SHADER

PostProcessing::PostProcessing() : Entity("PostProcessing") {
  pixelLength = 10;
  texture = LoadRenderTexture(2 * Border::length / pixelLength, 2 * Border::length / pixelLength);
  paletteShader = LoadShader(NULL, palletPath.c_str());
}

PostProcessing::~PostProcessing() {
  UnloadRenderTexture(texture);
  UnloadShader(paletteShader);
}

void PostProcessing::process(float delta) {
  if(2 * Border::length / pixelLength != texture.texture.height) {
    UnloadRenderTexture(texture);
    texture = LoadRenderTexture(2 * Border::length / pixelLength, 2 * Border::length / pixelLength);
  }
}

void PostProcessing::preRender() {
  BeginTextureMode(texture);
  BeginMode2D((Camera2D){
    .offset = {
      0,
      0
    },
    .target = {
      -Border::length,
      -Border::length
    },
    .rotation = 0,
    .zoom = 1 / pixelLength,
  });
}

void PostProcessing::postRender() {
  // DrawCircle(0, 0, 150, BLUE);
  EndMode2D();
  EndTextureMode();
#ifdef SHADER
  BeginShaderMode(paletteShader);
#endif

  Camera2D* cam = &Player::player->getCamera().camera;
  BeginMode2D(
    (Camera2D){
      .offset = cam->offset, 
      .target = cam->target - (Vector2){-Border::length, -Border::length}, 
      .rotation = 0,
      .zoom = cam->zoom
    }
  );

  unsigned int renderXCount = ceil((int)GetScreenWidth() / (Border::length * cam->zoom)) + 2;
  unsigned int renderYCount = ceil((int)GetScreenHeight() / (Border::length * cam->zoom)) + 2;
  DrawTexturePro(
    texture.texture,
    (Rectangle){
      0,
      0,
      renderXCount * (float)texture.texture.width,
      renderYCount * -(float)texture.texture.height
    }, 
    (Rectangle){
      -((int)(renderXCount / 2)) * Border::length * 2,
      -((int)(renderYCount / 2)) * Border::length * 2,
      renderXCount * Border::length * 2,
      renderYCount * Border::length * 2
    }, 
    {
      0, 
      0
    }, 
    0, 
    WHITE
  );
  EndMode2D();
#ifdef SHADER
  EndShaderMode();
#endif
}

void PostProcessing::postPostRender() {
}

Shader& PostProcessing::getShader() {
  return paletteShader;
}

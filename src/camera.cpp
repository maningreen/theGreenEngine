#include "border.hpp"
#include "camera.hpp"
#include "engine/entity.hpp"
#include "raylib.h"
#include "include.h"
#include <string>

float CameraEntity::smoothing = 10;
float CameraEntity::DefaultZoom = 0.3;
float CameraEntity::mouseLean = 0.1;

#define mouseLean .1

Vector2 CameraEntity::getMousePosition() {
  Vector2 mousePos = Vector2Scale(GetMousePosition() - camera.offset, 1.0f / camera.zoom);
  return mousePos + camera.target;
}

void CameraEntity::manageCameraShake(float delta) {
  Vector2 baseOffset = (Vector2){
    GetScreenWidth() / 2.0f,
    GetScreenHeight() / 2.0f
  };
  Vector2 mouseOffset = Vector2Scale(GetMousePosition() - camera.offset, -mouseLean);
  shakeVector = Vector2Normalize(shakeVector) + randomVector();
  shakeMag = Lerp(shakeMag, 0, smoothing * delta);
  camera.offset = baseOffset + mouseOffset + Vector2Scale(shakeVector, shakeMag);
}

void CameraEntity::manageCameraMotion(float delta) {
  if(follow == nullptr) 
    return;
  if(abs(camera.target.x - follow->position.x) >= Border::length) {
    if(camera.target.x > 0)
      camera.target.x -= 2 * Border::length;
    else if (camera.target.x < 0)
      camera.target.x += 2 * Border::length;
  } 
  if(abs(camera.target.y - follow->position.y) >= Border::length) {
    if(camera.target.y > 0)
      camera.target.y -= 2 * Border::length;
    else
      camera.target.y += 2 * Border::length;
  }
  camera.target = Vector2Lerp(camera.target, follow->position, smoothing * delta);
}

void CameraEntity::process(float delta) {
  manageCameraMotion(delta);
  manageCameraShake(delta);
}

void CameraEntity::render() {
  ClearBackground(BLACK);
}

void CameraEntity::applyShake(float magnitude) {
  shakeMag += magnitude;
}

CameraEntity::~CameraEntity() {}

CameraEntity::CameraEntity(std::string name, Entity2D* target) : Entity(name) {
  follow = target;
  camera = {{0, 0}, {0, 0}, 0, DefaultZoom};
  shakeVector = (Vector2){0, 0};
}

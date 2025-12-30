#include "border.hpp"
#include "camera.hpp"
#include "engine/entity.hpp"
#include "raylib.h"
#include <string>

float CameraEntity::Smoothing = 10;
float CameraEntity::DefaultZoom = 0.3;
float CameraEntity::mouseLean = 0.1;
float smoothing = 0.15;

#define mouseLean .1

Vector2 CameraEntity::getMousePosition() {
  Vector2 mousePos = Vector2Scale(GetMousePosition() - camera.offset, 1.0f / camera.zoom);
  return mousePos + camera.target;
}

void CameraEntity::manageCameraShake(float delta) {
  Vector2 baseOff = (Vector2){GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f};
  camera.offset = Vector2Add(baseOff, Vector2Scale(Vector2Subtract(GetMousePosition(), camera.offset), -mouseLean));
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
  camera.target = Vector2Lerp(camera.target, follow->position, smoothing);
}

void CameraEntity::process(float delta) {
  manageCameraMotion(delta);
  manageCameraShake(delta);
}

void CameraEntity::render() {
  ClearBackground(BLACK);
}

CameraEntity::~CameraEntity() {}

CameraEntity::CameraEntity(std::string name, Entity2D* target) : Entity(name) {
  follow = target;
  camera = {{0, 0}, {0, 0}, 0, DefaultZoom};
  jitterness = 0;
  shakeVector = (Vector2){0, 0};
}

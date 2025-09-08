#include "camera.hpp"
#include "engine/entity.hpp"
#include <iostream>
#include "include.h"
#include <string>

float CameraEntity::Smoothing = 10;
float CameraEntity::DefaultZoom = .3;

#define mouseLean .1

void CameraEntity::ManageCameraShake(float delta) {
  Vector2 baseOff = (Vector2){GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f};
  Camera.offset = Vector2Add(baseOff, Vector2Scale(Vector2Subtract(GetMousePosition(), Camera.offset), -mouseLean));
}

void CameraEntity::ManageCameraMotion(float delta) {
  if(Follow != nullptr)
    Camera.target = *Follow;
}

void CameraEntity::Process(float delta) {
  ManageCameraMotion(delta);
  ManageCameraShake(delta);
}

void CameraEntity::Render() {
  ClearBackground(BLACK);
}

CameraEntity::~CameraEntity() {}

CameraEntity::CameraEntity(std::string name, Vector2* target) : Entity(name) {
  Follow = target;
  Camera = {{0, 0}, {0, 0}, 0, DefaultZoom};
  Jitterness = 0;
  ShakeVector = (Vector2){0, 0};
}

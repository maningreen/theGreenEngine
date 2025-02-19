#include "camera.hpp"
#include "entity.hpp"
#include <iostream>
#include <raylib.h>
#include <raymath.h>
#include <string>

float CameraEntity::Smoothing = 10;
float CameraEntity::DefaultZoom = .9;

#define mouseLean .3

void CameraEntity::ManageCameraShake(float delta) {
  Camera.target = Vector2Add(Target, Vector2Scale(Vector2Subtract(GetMousePosition(), Camera.offset), mouseLean));
}

void CameraEntity::ManageCameraMotion(float delta) {
  Target = Vector2Lerp(Target, *Follow, Smoothing * delta);
}

void CameraEntity::Process(float delta) {
  ManageCameraMotion(delta);
  ManageCameraShake(delta);
  Camera.offset = (Vector2){GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f};
}

void CameraEntity::Render() {
  BeginMode2D(Camera);
  ClearBackground(BLACK);
}

CameraEntity::~CameraEntity() {}

CameraEntity::CameraEntity(std::string name, Vector2* target) : Entity(name) {
  Follow = target;
  Camera = {{0, 0}, {0, 0}, 0, DefaultZoom};
  Jitterness = 0;
  ShakeVector = (Vector2){0, 0};
  Target = (Vector2){0, 0};
}

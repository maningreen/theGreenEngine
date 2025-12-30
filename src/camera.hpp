#ifndef camera_hpp
#define camera_hpp

#include <string>
#include "engine/entity.hpp"

class CameraEntity : public Entity {
private:
  void manageCameraShake(float delta);
  void manageCameraMotion(float delta);

public:
  Camera2D camera;
  Entity2D* follow;
  Vector2 shakeVector;
  float jitterness;

  Vector2 getMousePosition();

  void process(float delta);
  void render();

  CameraEntity(std::string name, Entity2D* target);
  ~CameraEntity();

  static float Smoothing;
  static float DefaultZoom;
  static float mouseLean;
};

#endif

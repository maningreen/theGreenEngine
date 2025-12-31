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
  float shakeMag;

  Vector2 getMousePosition();

  void process(float delta);
  void render();

  // magnitude is in px.
  void applyShake(float magnitude);

  CameraEntity(std::string name, Entity2D* target);
  ~CameraEntity();

  static float smoothing;
  static float DefaultZoom;
  static float mouseLean;
};

#endif

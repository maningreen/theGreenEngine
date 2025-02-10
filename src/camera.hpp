#ifndef camera_hpp
#define camera_hpp

#include <raylib.h>
#include <string>
#include "entity.hpp"

class CameraEntity : public Entity {
  public:
    Camera2D Camera;
    Vector2 Target;
    Vector2* Follow;
    Vector2 ShakeVector;
    float Jitterness;

    void ManageCameraShake(float delta);
    void ManageCameraMotion(float delta);

    void Process(float delta);
    void Render();

    CameraEntity(std::string name, Vector2* target);

    static float Smoothing;
};

#endif

#ifndef camera_hpp
#define camera_hpp

#include "include.h"
#include <string>
#include "engine/entity.hpp"

class CameraEntity : public Entity {
  public:
    Camera2D Camera;
    Entity2D* Follow;
    Vector2 ShakeVector;
    float Jitterness;

    void ManageCameraShake(float delta);
    void ManageCameraMotion(float delta);

    void Process(float delta);
    void Render();

    CameraEntity(std::string name, Entity2D* target);
    ~CameraEntity();

    static float Smoothing;
    static float DefaultZoom;
};

#endif

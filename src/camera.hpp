#ifndef camera_hpp
#define camera_hpp

#include <string>
#include "engine/entity.hpp"

class CameraEntity : public Entity {
  public:
    Camera2D Camera;
    Entity2D* Follow;
    Vector2 ShakeVector;
    float Jitterness;

    Vector2 getMousePosition();

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

#ifndef player_h
#define player_h

#include "entity.hpp"
#include "bars.hpp"
#include "camera.hpp"
#include <raylib.h>

class Player : public Entity2D {
  private:
    bool dashing;
    bool canDash;
    float timeDashing;
    float timeSinceDash;
    Vector2 dashDirection;

    Bar* dashBar;
    Bar* dashCooldownBar;

    CameraEntity* cam;

    void manageBars();

    void manageRotation();

  public:
    Vector2 Velocity;

    float Speed;
    float Friction;

    float Rotation;

    static const int upKey;
    static const int downKey;
    static const int leftKey;
    static const int rightKey;
    static const int shootKey;
    static const int shootKeyMouse;
    static const int dashKey;
    static const float defaultSpeed;
    static const float defaultFriction;

    static float dashSpeed;
    static float dashTime;
    static float dashControl;

    static float dashCooldown;

    virtual void Process(float delta);
    virtual void Render();

    Player(const std::string& name, Vector2 position, CameraEntity* cam);

    Vector2 getInput();
    Vector2 getInput(int u, int d, int l, int r);

    void setCam(CameraEntity* cam);

    void wrapPosition();

    void SpawnBullet();
};

#endif

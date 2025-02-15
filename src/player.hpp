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
    float timeSinceDash;
    Vector2 dashDirection;

    float lifetime;

    Bar* dashBar;
    Bar* dashCooldownBar;

    CameraEntity* cam;

    void manageBars();

    void manageBar(Bar* b, int offsetCount, float p, bool shouldRender);

    void manageRotation();

  public:
    Vector2 Velocity;

    float Speed;
    float Friction;

    float Rotation;

    static int upKey;
    static int downKey;
    static int leftKey;
    static int rightKey;
    static int shootKey;
    static int shootKeyMouse;
    static int dashKey;
    static const float defaultSpeed;
    static const float defaultFriction;

    static float dashSpeed;
    static float dashTime;
    static float dashControl;

    static float dashCooldown;

    static float particleSpawnTime;

    void Process(float delta) override;
    void Render() override;
    void Init() override;

    Player(const std::string& name, Vector2 position, CameraEntity* cam);

    Vector2 getInput();
    Vector2 getInput(int u, int d, int l, int r);

    void setCam(CameraEntity* cam);

    void wrapPosition();

    void SpawnBullet();

    float getLifetime();
};

#endif

#ifndef player_h
#define player_h

#include "engine/entity.hpp"
#include "bars.hpp"
#include "camera.hpp"
#include "healthManager.hpp"
#include "include.h"

class Player : public Entity2D {
private:
  bool dashing;
  bool canDash;
  float timeSinceDash;
  Vector2 dashDirection;

  float lifetime;

  Bar* dashCooldownBar;
  HealthManager* healthManager;

  CameraEntity* cam;

  void manageBars();

  void manageBar(Bar* b, int offsetCount, float p, bool shouldRender);

  void manageRotation();

public:
  Vector2 Velocity;

  float Speed;
  float Friction;

  float Rotation;

  unsigned dashCount;
  float dashProgress;

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

  static unsigned maxDashCount;

  static float dashCooldown;

  static float particleSpawnTime;

  static float hitboxRadius;

  void Process(float delta) override;
  void Render() override;
  void Init() override;

  Player(const std::string& name, Vector2 position, CameraEntity* cam);
  ~Player();

  Vector2 getInput();
  Vector2 getInput(int u, int d, int l, int r);

  void setCam(CameraEntity* cam);

  void SpawnBullet();

  HealthManager* getHealthManager();

  float getLifetime();
  bool getDashing();
};

#endif

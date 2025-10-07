#ifndef player_h
#define player_h

#include "bars.hpp"
#include "camera.hpp"
#include "enemy.hpp"
#include "engine/entity.hpp"
#include "healthManager.hpp"

class Player : public Entity2D {
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

  void manageAttack();
public:
  Vector2 Velocity;

  float Speed;
  float Friction;

  float Rotation;

  unsigned dashCount;
  float dashProgress;

  static float maxHealth;

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
  static float dashRegenDelay;

  static unsigned maxDashCount;

  static float dashCooldown;

  static float particleSpawnTime;

  static float hitboxRadius;

  void Process(float delta) override;
  void Render() override;
  void Init() override;

  static bool addEnemy(Enemy*);
  static bool removeEnemy(Enemy*);

  Player(const std::string& name, Vector2 position, CameraEntity* cam);
  ~Player();

  Vector2 getInput();
  Vector2 getInput(int u, int d, int l, int r);

  HealthManager* getHealthManager();

  float getLifetime();
  bool getDashing();
};

#endif

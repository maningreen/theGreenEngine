#ifndef player_h
#define player_h

#include "bars.hpp"
#include "camera.hpp"
#include "enemy.hpp"
#include "engine/entity.hpp"
#include "healthManager.hpp"
#include "include.h"
#include "inputManager.hpp"
#include "mod.hpp"
#include "dashManager.hpp"

class Player : public Entity2D {
  private:
    float lifetime;

    Bar* dashCooldownBar;

    HealthManager* healthManager;
    DashManager dashManager;
    InputManager* inputManager;

    CameraEntity* cam;

    ModManager* modManager;

    void manageBars();

    void manageBar(Bar* b, int offsetCount, float p, bool shouldRender);

    void manageRotation();

    void manageDash(float delta);

    void manageInput(float delta, Vector2 input);

    void beginDash(Vector2 input);

public:
  Player(const std::string& name, Vector2 position, CameraEntity* cam);
  ~Player();

  Vector2 velocity;

  float speed;
  float friction;

  float rotation;

  void fireBullet();

  void Process(float delta) override;
  void Render() override;
  void Init() override;
  float getLifetime();

  Vector2 getInput();

  HealthManager* getHealthManager();
  DashManager* getDashManager();

  static bool addEnemy(Enemy*);
  static bool removeEnemy(Enemy*);

  static float maxHealth;

  static Key upKey;
  static Key downKey;
  static Key leftKey;
  static Key rightKey;
  static Key shootKey;
  static Key shootKeyMouse;
  static Key dashKey;

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
};

#endif

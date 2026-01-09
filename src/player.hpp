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
    static InputManager* inputManager;

    CameraEntity* cam;

    ModManager* modManager;

    void manageBars();

    void manageBar(Bar* b, int offsetCount, float p, bool shouldRender);

    void manageRotation();

    void manageDash(float delta);

    void manageInput(float delta, Vector2 input);

    void beginDash(Vector2 input);

public:
  Player(const std::string& name, Vector2 position);
  ~Player();

  Vector2 velocity;

  float speed;

  float rotation;

  void fireBullet();

  void process(float delta) override;
  void render() override;
  void init() override;
  float getLifetime();

  Vector2 getInput();

  HealthManager& getHealthManager();
  DashManager& getDashManager();
  CameraEntity& getCamera();
  ModManager& getModManager();
  static InputManager* getInputManager();

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

  static const float defaultShakeMag;
  static float shakeMag;

  static const float defaultDashSpeed;
  static const float defaultDashTime;
  static const float defaultDashControl;
  static const float defaultDashRegenDelay;
  static const unsigned defaultMaxDashCount;
  static const float defaultDashCooldown;

  static float particleSpawnTime;

  static float hitboxRadius;

  static Player* player;
};

#endif

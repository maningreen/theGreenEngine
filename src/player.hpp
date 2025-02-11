#include "entity.hpp"
#include <raylib.h>

class Player : public Entity2D {
  private:
    bool dashing;
    float timeDashing;
    Vector2 dashDirection;

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

    virtual void Process(float delta);
    virtual void Render();

    Player(const std::string& name, Vector2 position);

    Vector2 getInput();
    Vector2 getInput(int u, int d, int l, int r);

    void wrapPosition();

    void SpawnBullet();
};

#include "entity.hpp"
#include <raylib.h>

class Player : public Entity2D {
  public:
    Vector2 Velocity;

    float Speed;
    float Friction;

    static const int upKey;
    static const int downKey;
    static const int leftKey;
    static const int rightKey;
    static const int shootKey;
    static const float defaultSpeed;
    static const float defaultFriction;

    virtual void Process(float delta);
    virtual void Render();

    Player(const std::string& name, Vector2 position);

    Vector2 getInput();
    Vector2 getInput(int u, int d, int l, int r);

    void wrapPosition();
};

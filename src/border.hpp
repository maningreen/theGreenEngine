#ifndef border_h
#define border_h

#include "engine/entity.hpp"

class Border : public Entity2D {
  public:
    static float Length;

    virtual void Render();
    virtual void Process(float delta);

    Border();
    ~Border();

    static void wrapEntity(Entity2D* en);

    static Vector2 wrapPos(Vector2 vector);

    static Vector2 getShortestPathToPoint(Entity2D* en, Vector2 position);

    static Vector2 getRandomPosInBorder();
};

#endif

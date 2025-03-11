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
    static Vector2 wrapPosX(Vector2 v);
    static Vector2 wrapPosY(Vector2 v);
    static Vector2 getShortestPathToPoint(const Entity2D* en, const Vector2 position);
    static Vector2 getShortestPathToPoint(const Vector2 pos1, const Vector2 pos2);
    static Vector2 getRandomPosInBorder();
    static float getDistance(Vector2 a, Vector2 b);
};

#endif

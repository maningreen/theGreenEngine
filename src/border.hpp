#ifndef border_h
#define border_h

#include "engine/entity.hpp"

// this class is used to represent the wrapping of geometry on a taurus (3D donut).
// since it's a taurus there's some... funky geometry... (Shortest paths are still shortest paths, just with wrapping applied)
// it's a static class* *(exception being rendering)
// and so most of the functions and values in here are static.
class Border : public Entity2D {
  public:
    static float length;

    virtual void render();

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

    // where beta is the one to be unwrapped
    // if there's a shorter path that "loops around", b will be placed relative to alpha
    // does *NOT* return a relative position
    static Vector2 unwrapPositionRelative(Vector2 alpha, Vector2 beta);
};

#endif

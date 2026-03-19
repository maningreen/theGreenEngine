#include "laser.hpp"

#include <cmath>

#include "border.hpp"
#include "engine/entity.hpp"
#include "include.h"

extern "C" {
void drawLaser(float length, float theta, Vector2 pos, float wrap, Color col);
}

#define cot(x) (1 / tan(x))

Color Laser::defaultColour = WHITE;
float Laser::defaultLength = 100;
float Laser::defaultWidth = 20;

Laser::Laser(Vector2 p, float rot, float len, Color col)
  : Entity2D("Laser", p),
    rotation(rot),
    length(len),
    colour(col),
    shouldRender(true),
    width(defaultWidth) {}
Laser::Laser(Vector2 p, float rot, float len)
  : Entity2D("Laser", p),
    rotation(rot),
    length(len),
    colour(defaultColour),
    shouldRender(true),
    width(defaultWidth) {}
Laser::Laser(Vector2 p, float rot)
  : Entity2D("Laser", p),
    rotation(rot),
    length(defaultLength),
    colour(defaultColour),
    shouldRender(true),
    width(defaultWidth) {}
Laser::Laser(Vector2 p)
  : Entity2D("Laser", p),
    rotation(0),
    length(defaultLength),
    colour(defaultColour),
    shouldRender(true),
    width(defaultWidth) {}

Color Laser::getDefaultColour() {
    return defaultColour;
}

float Laser::getDefaultLength() {
    return defaultLength;
}

float Laser::getDefaultWidth() {
    return defaultWidth;
}

void Laser::lookAt(Vector2 v) {
    Vector2 vecTo = Border::getShortestPathToPoint(position, v);
    rotation = atan2f(vecTo.y, vecTo.x);
    length = Vector2Length(vecTo);
}

bool Laser::getBreaks() {
    return getBreaksX() || getBreaksY();
}

bool Laser::getBreaksX() {
    Vector2 localOffset = (Vector2){cosf(rotation) * length, sinf(rotation) * length};
    Vector2 preWrap = Vector2Add(position, localOffset);
    Vector2 endPos = Border::wrapPos(preWrap);
    return endPos.x != preWrap.x;
}

bool Laser::getBreaksY() {
    Vector2 localOffset = (Vector2){cosf(rotation) * length, sinf(rotation) * length};
    Vector2 preWrap = Vector2Add(position, localOffset);
    Vector2 endPos = Border::wrapPos(preWrap);
    return endPos.y != preWrap.y;
}

Vector2 Laser::getBreakingPoint() {
    if(!getBreaks()) return Vector2Zero();
    Vector2 endPos = getEndPoint();
    float slope = (endPos.y - position.y) / (endPos.x - position.x);
    float xLen, yLen;
    if(getBreaksX()) {
        xLen = Border::length - abs(position.x);
        yLen = xLen * slope + position.y;
    } else if(getBreaksY()) {
        yLen = Border::length - abs(position.y);
        xLen = xLen / slope + position.y;
    }
    return (Vector2){position.x + xLen, position.y + yLen};
}

Vector2 Laser::getEndPoint() {
    Vector2 localOffset = (Vector2){cosf(rotation) * length, sinf(rotation) * length};
    return Border::wrapPos(Vector2Add(position, localOffset));
}

void Laser::render() {
    if(!shouldRender)
        return;
    drawLaser(length, rotation, position, Border::length, colour);
}

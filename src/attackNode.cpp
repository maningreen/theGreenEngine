#include "attackNode.hpp"
#include "border.hpp"
#include "engine/entity.hpp"
#include "include.h"
#include "laser.hpp"
#include "player.hpp"
#include "raylib.h"
#include "raymath.h"
#include <cmath>
#include <vector>
#include "engine/core.h"

#define min(a, b) (a < b ? a : b)

extern "C" {
float getDistanceFromLineAndPoint(float aX, float aY, float bX, float bY,
    float cX, float cY);
void getClosestPointFromLineAndPoint(Vector2* a, Vector2* b, Vector2* c,
    Vector2* out);
};


float AttackNode::defaultRadius = 30;
float AttackNode::lifetimeAfterAttack = 1.5;
std::vector<AttackNode*> AttackNode::nodes;

float AttackNode::damage = 60000;

AttackNode::AttackNode(Vector2 p)
    : Entity2D("DashNode", p), lifetime(0), radius(defaultRadius) {
  las = new Laser(Position, 0, 500, WHITE);
  las->shouldRender = false;
  addChild(las);
  index = nodes.size();
  nodes.push_back(this);
}

AttackNode::AttackNode(Vector2 p, bool x)
    : Entity2D("DashNode", p), lifetime(0), radius(defaultRadius) {
  las = new Laser(Position, 0, 500, WHITE);
  las->shouldRender = false;
  addChild(las);
}

AttackNode::~AttackNode() {
  for(int i = 0; i < nodes.size(); i++)
    if(nodes[i] == this)
      nodes.erase(nodes.begin() + i);
}

float AttackNode::ease(float x) { return 1 - (x * x * x * x); }

int AttackNode::getIndex() { 
  for(int i = 0; i < nodes.size(); i++)
    if(nodes[i] == this)
      return i;
  return -1;
}

float AttackNode::getMaxLifetime() { return 2 * Player::player->getDashManager()->regenRate; }

std::vector<AttackNode*> AttackNode::getNodes() { return nodes; }

AttackNode* AttackNode::getPrev() {
  return nodes[((index + nodes.size() - 1) % nodes.size())];
}

AttackNode* AttackNode::getNext() { return nodes[((index + 1) % nodes.size())]; }

float AttackNode::getLasAngle() { return las->rotation; }

bool AttackNode::getBreakInLas() { return las->getBreaks(); }

float AttackNode::getInternalAngle() {
  if(nodes.size() < 3)
    return 0;
  float thetaA = getLasAngle();
  float thetaB = fmodf(getPrev()->getLasAngle() + PI, PI * 2);
  float deltaTheta = fmodf(thetaB - thetaA, PI * 2);
  if(deltaTheta < PI)
    return deltaTheta;
  else
    return 2 * PI - deltaTheta;
}

float AttackNode::getInternalAngleSum() {
  if(nodes.size() < 3)
    return -1;
  float theta = 0;
  for(int i = 0; i < nodes.size(); i++)
    theta += nodes[i]->getInternalAngle();
  return theta;
}

int AttackNode::getBreakInPolygon() {
  // THERE IS A SINGULAR EDGE CASE
  // that's if the first index is the one that's breaking it
  if(nodes[0]->getPrev()->getBreakInLas())
    return 0;
  for(int i = 0; i < nodes.size(); i++) {
    if(nodes[i]->getBreakInLas()) {
      if(nodes[i]->getNext()->getBreakInLas())
        return i + 1;
    }
  }
  return -1;
}

AttackNode AttackNode::unwrapRelative() {
  AttackNode x = *this;
  if(abs(x.Position.x - getPrev()->Position.x) > Border::length) {
    if(x.Position.x > 0)
      x.Position.x -= Border::length * 2;
    else
      x.Position.x += Border::length * 2;
  } else if(abs(x.Position.y - getPrev()->Position.y) > Border::length) {
    if(x.Position.y > 0)
      x.Position.y -= Border::length * 2;
    else
      x.Position.y += Border::length * 2;
  }
  return x;
}

// returns 0 if invalid triangle
float AttackNode::getArea() {
  if(nodes.size() < 3)
    return 0;
  if(getTriangleIsRegular())
    return calculateTriangleArea(nodes[0]->Position, nodes[1]->Position, nodes[2]->Position);
  return 0;
}

float AttackNode::getArea(float theta) {
  if(nodes.size() < 3)
    return 0;
  if(getTriangleIsRegular(theta))
    return calculateTriangleArea(nodes[0]->Position, nodes[1]->Position, nodes[2]->Position);
  return 0;
}

bool AttackNode::getTriangleIsRegular() {
  float angleSum = getInternalAngleSum();
  return getTriangleIsValid(angleSum);
}

bool AttackNode::getTriangleIsRegular(float angleSum) {
  return getTriangleIsValid(angleSum);
}

void AttackNode::Render() {
  DrawCircleV(Position, radius, WHITE);
}

void AttackNode::Process(float delta) {
  lifetime += delta;

  if(nodes.size() >= 3) {
    AttackNode* next = getNext();
    Vector2 vectorToNext = Border::getShortestPathToPoint(this, next->Position);
    if(!las->shouldRender) {
      las->shouldRender = true;
      lifetime = Player::player->getDashManager()->regenRate + -lifetimeAfterAttack;
      las->lookAt(next->Position);
      radius = defaultRadius;
    }
    las->length = Vector2Length(vectorToNext) - (next->radius + radius);
    las->width = sqrt(radius);
    // then we wanna ummm crap, offset the umm mfrickennnaaahahhmmmm the
    // las->position yeah
    las->Position = Position + Vector2Scale(Vector2Normalize(vectorToNext), radius);
    if(index == 0)
      manageAttack();
  } else if(lifetime <= 1)
    radius = ease(1 - lifetime) * defaultRadius;
  if(lifetime >= getMaxLifetime())
    radius = ease(lifetime - getMaxLifetime()) * defaultRadius;

  // the +1 accounts for the time to ease out
  if(lifetime >= getMaxLifetime() + 1)
    killDefered();
}

void AttackNode::manageAttack() {
  if(nodes.size() < 3)
    return;

  float area = getArea();
  bool validTriangle = getTriangleIsRegular();

  std::vector<Entity*> enemies =
      Engine::getAllChildrenWithTagRecursive(getRoot(), Enemy::tag);

  for(int i = 0; i < enemies.size(); i++) {
    Enemy* en = (Enemy*)enemies[i];
    // we figure out the function for the actual slope thingy
    if(validTriangle) {
      // so in this the triangle is regular, so we want to treat it like one;
      // however, sometimes it needs to be unwrapped, otherwise it's considered
      // having a much larger area than it should

      Vector2 effectivePos[nodes.size()];
      for(int i = 0; i < nodes.size(); i++)
        effectivePos[i] =
            Border::unwrapPositionRelative(en->Position, nodes[i]->Position);

      // then we can use a regular collision
      // checking collision circle triangle, easy peasy, lemon squeazy
      Vector2 avg = Vector2Zero();
      for(int i = 0; i < 3; i++)
        avg = Vector2Add(avg, effectivePos[i]);
      avg = Vector2Scale(avg, 1.0f / 3.0f);

      Vector2 vecToAvg = Vector2Subtract(avg, en->Position);
      float dist = Vector2Length(vecToAvg);
      float r = en->radius;

      float min = dist < r ? dist : r;
      Vector2 p = Vector2Add(en->Position, Vector2Scale(vecToAvg, min / dist));
      if(CheckCollisionPointTriangle(p,
             effectivePos[0],
             effectivePos[1],
             effectivePos[2]))
        ((Enemy*)en)->getHealthManager()->applyDamage(damage / area);
    } else {
      float minimumDistance = INFINITY;
      for(int i = 0; i < 3; i++) {
        Vector2 unwrappedNext = nodes[i]->getNext()->unwrapRelative().Position;
        Vector2 closestPoint;

        // this is an external function written in haskell
        getClosestPointFromLineAndPoint(&nodes[i]->Position,
            &unwrappedNext,
            &en->Position,
            &closestPoint);
        float distance = Vector2Distance(closestPoint, en->Position);
        minimumDistance = min(distance, minimumDistance);
      }
      if(minimumDistance <= en->radius)
        en->killDefered();
    }
  }
}

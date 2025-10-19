#include "attackNode.hpp"
#include "border.hpp"
#include "engine/entity.hpp"
#include "laser.hpp"
#include "player.hpp"
#include "raylib.h"
#include "raymath.h"
#include <cmath>
#include <vector>

#define min(a, b) (a < b ? a : b)
#define max(a, b) (a > b ? a : b)

float AttackNode::defaultRadius = 30;
float AttackNode::lifetimeAfterAttack = 1.5;
std::vector<AttackNode*> AttackNode::nodes;

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

int AttackNode::getIndex() { return index; }

float AttackNode::getMaxLifetime() { return 2 * Player::dashCooldown; }

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

int AttackNode::getBreakInPolygon() {
  for(int i = 0; i < nodes.size(); i++) {
    if(nodes[i]->getBreakInLas()) {
      // THERE IS A SINGULAR EDGE CASE
      // that's if the first index is the one that's breaking it
      if(nodes[i]->getPrev()->getBreakInLas())
        return i;
      else if(nodes[i]->getNext()->getBreakInLas())
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
  }
  if(abs(x.Position.y - getPrev()->Position.y) > Border::length) {
    if(x.Position.y > 0)
      x.Position.y -= Border::length * 2;
    else
      x.Position.y += Border::length * 2;
  }
  return x;
}

void AttackNode::Render() {
  DrawCircleGradient(Position.x, Position.y, radius, BLANK, WHITE);
  // DrawCircleGradient(int centerX, int centerY, float radius, Color inner,
  // Color outer) DrawCircleLinesV(Vector2 center, float radius, Color color)
}

void AttackNode::Process(float delta) {
  lifetime += delta;

  if(lifetime >= getMaxLifetime())
    radius = ease(lifetime - getMaxLifetime()) * defaultRadius;
  else if(lifetime <= 1)
    radius = ease(1 - lifetime) * defaultRadius;

  if(nodes.size() >= 3) {
    AttackNode* next = getNext();
    Vector2 vectorToNext = Border::getShortestPathToPoint(this, next->Position);
    if(!las->shouldRender) {
      las->shouldRender = true;
      lifetime = Player::dashCooldown + -lifetimeAfterAttack;
      las->lookAt(next->Position);
    }
    las->length = Vector2Length(vectorToNext) - (next->radius + radius);
    las->width = sqrt(radius);
    // then we wanna ummm crap, offset the umm mfrickennnaaahahhmmmm the
    // las->position yeah
    las->Position = Position + Vector2Scale(Vector2Normalize(vectorToNext), radius);
  }

  if(lifetime >= getMaxLifetime() + 1)
    killDefered();
}

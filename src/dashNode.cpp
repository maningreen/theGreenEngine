#include "dashNode.hpp"
#include "engine/entity.hpp"
#include "laser.hpp"
#include "player.hpp"
#include "border.hpp"
#include <cmath>
#include <raylib.h>
#include <raymath.h>
#include <vector>

float DashNode::defaultRadius = 30;
float DashNode::lifetimeAfterAttack = 1.5;
std::vector<DashNode*> DashNode::nodes;

DashNode::DashNode(Vector2 p) : Entity2D("DashNode", p), lifetime(0), radius(defaultRadius) {
  las = new Laser(Position, 0, 500, WHITE);
  las->shouldRender = false;
  addChild(las);
  index = nodes.size();
  nodes.push_back(this);
}

DashNode::~DashNode() {
  for(int i = 0; i < nodes.size(); i++)
    if(nodes[i] == this)
      nodes.erase(nodes.begin() + i);
}

float DashNode::ease(float x) {
  return 1 - (x * x * x * x);
}

int DashNode::getIndex() {
  return index;
}

float DashNode::getMaxLifetime() {
  return 2 * Player::dashCooldown;
}


std::vector<DashNode*> DashNode::getNodes() {
  return nodes;
}

DashNode* DashNode::getPrev() {
  return nodes[((index + nodes.size() - 1) % nodes.size())];
}

DashNode* DashNode::getNext() {
  return nodes[((index + 1) % nodes.size())];
}

float DashNode::getLasAngle() {
  return las->rotation;
}

bool DashNode::getBreakInLas() {
  return las->getBreaks();
}

int DashNode::getBreakInPolygon() {
  if(nodes.size() < 3) return false;

  for(DashNode* n : nodes) {
    if(n->getBreakInLas()) {
      // k, if it breaks, lets check if the previous one breaks
      if(n->getPrev()->getBreakInLas())
        return n->getIndex();
      else
        return n->getNext()->getIndex();
      // there's an edgecase here that the if accounts for, that is if the break-ee is index 0
    }
  }
  return -1;
}

float DashNode::getInternalAngle() {
  if(nodes.size() < 3) return 0;
  float prevA = 180 + (getPrev()->getLasAngle() * 180 / M_PI);
  prevA -= floor(prevA / 360) * 360;
  float angleDeg = las->rotation * 180 / M_PI;
  float diff = angleDeg - prevA;
  diff -= floor(diff / 360) * 360;
  return diff;
}

void DashNode::Render() {
  DrawCircleGradient(Position.x, Position.y , radius, BLANK, WHITE);
  // DrawCircleGradient(int centerX, int centerY, float radius, Color inner, Color outer)
  // DrawCircleLinesV(Vector2 center, float radius, Color color)
  if(nodes.size() >= 3) {
    float angleDeg = las->rotation * 180 / M_PI;
    float prevA = fmodf(180 + getPrev()->getLasAngle() * 180 / M_PI, 360);
    DrawCircleSector(Position, 100, prevA, angleDeg, 100, RED);
  }
}

void DashNode::Process(float delta) {
  lifetime += delta;

  // if(lifetime >= getMaxLifetime())
    // radius = ease(lifetime - getMaxLifetime()) * defaultRadius;
  // else
  if(lifetime <= 1)
    radius = ease(1 - lifetime) * defaultRadius;

  if(nodes.size() >= 3) {
    DashNode* next = getNext();
    Vector2 vectorToNext = Border::getShortestPathToPoint(this, next->Position);
    if(!las->shouldRender) {
      las->shouldRender = true;
      lifetime = Player::dashCooldown + - lifetimeAfterAttack;
      // we gotta turn "next" into local positions
      las->lookAt(next->Position);
    }
    las->length = Vector2Length(vectorToNext) - (next->radius + radius);
    las->width = sqrt(radius);
    // then we wanna ummm crap, offset the umm mfrickennnaaahahhmmmm the las->position yeah
    las->Position = Vector2Add(Position, Vector2Scale(vectorToNext, radius / las->length));
  }

  // if(lifetime >= getMaxLifetime() + 1) 
  // killDefered();
}

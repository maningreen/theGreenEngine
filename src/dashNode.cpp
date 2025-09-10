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
  for(int i = 0; i < nodes.size(); i++)
    if(nodes[i] == this)
      return i;
  return -1;
}

float DashNode::getMaxLifetime() {
  return 2 * Player::dashCooldown;
}


std::vector<DashNode*> DashNode::getNodes() {
  return nodes;
}

void DashNode::Render() {
  DrawCircleLinesV(Position, radius, WHITE);
}

void DashNode::Process(float delta) {
  lifetime += delta;

  if(lifetime >= getMaxLifetime())
    radius = ease(lifetime - getMaxLifetime()) * defaultRadius;
  else if(lifetime <= 1)
    radius = ease(1 - lifetime) * defaultRadius;

  if(nodes.size() >= 3) {
    int index = getIndex();
    DashNode* next = nodes[(index + 1) % nodes.size()];
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

  if(lifetime >= getMaxLifetime() + 1) 
    killDefered();
}

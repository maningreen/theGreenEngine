#include "dashNode.hpp"
#include "engine/entity.hpp"
#include "laser.hpp"
#include "player.hpp"
#include <cmath>
#include <raylib.h>
#include <raymath.h>
#include <vector>

float laserWidth = 5;

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
  return x * x * x * x;
}

int DashNode::getIndex() {
  for(int i = 0; i < nodes.size(); i++)
    if(nodes[i] == this)
      return i;
  return -1;
}

void DashNode::Render() {
  DrawCircleLinesV(Position, radius, WHITE);
}

void DashNode::Process(float delta) {
  lifetime += delta;
  radius = lifetime >= Player::dashCooldown ? (1 - ease(lifetime - Player::dashCooldown - Player::dashRegenDelay)) * defaultRadius : radius;
  if(nodes.size() >= 3) {
    int index = getIndex();
    DashNode* next = nodes[(index + 1) % nodes.size()];
    if(!las->shouldRender) {
      las->shouldRender = true;
      lifetime = Player::dashCooldown + Player::dashRegenDelay - lifetimeAfterAttack;
    }
    Vector2 vectorToNext = Vector2Subtract(next->Position, Position);
    las->length = Vector2Length(vectorToNext) - (next->radius + radius);
    las->width = laserWidth * radius / defaultRadius;
    las->rotation = atan2f(vectorToNext.y, vectorToNext.x);
    // then we wanna ummm crap, offset the umm mfrickennnaaahahhmmmm the las->position yeah
    las->Position = Vector2Add(Position, Vector2Scale(vectorToNext, radius / las->length));
  }
  if(lifetime >= Player::dashCooldown + Player::dashRegenDelay + 1) 
    killDefered();
}

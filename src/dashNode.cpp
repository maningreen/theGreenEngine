#include "dashNode.hpp"
#include "engine/entity.hpp"
#include "player.hpp"
#include <cmath>
#include <raylib.h>
#include <raymath.h>

float DashNode::radius = 30;

DashNode::DashNode(Vector2 p) : Entity2D("DashNode", p), lifetime(0) {}

float DashNode::ease(float x) {
  return x * x * x * x;
}

void DashNode::Render() {
  float r = lifetime >= Player::dashCooldown ? (1 - ease(lifetime - Player::dashCooldown - Player::dashRegenDelay)) * radius : radius;
  DrawCircleLinesV(Position, r, WHITE);
}

void DashNode::Process(float delta) {
  lifetime += delta;
  if(lifetime >= Player::dashCooldown + Player::dashRegenDelay + 1)
    killDefered();
}

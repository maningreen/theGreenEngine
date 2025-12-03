#include "nodeBullet.hpp"
#include "border.hpp"
#include "engine/entity.hpp"
#include "raylib.h"
#include "attackNode.hpp"
#include "raymath.h"

float NodeBullet::speed = 5000;
float NodeBullet::radius = 10;
Color NodeBullet::color = WHITE;

const std::string NodeBullet::tag = "nodeBullet";

NodeBullet::NodeBullet(Vector2 alpha, Vector2 beta, float r)
    : Entity2D("NodeBullet", alpha), theta(r) {
  // calculate the lifetime with the speed and distance
  targetLifetime = Vector2Distance(alpha, beta) / speed;
  lifetime = 0;
  addTag(tag);
}

NodeBullet::NodeBullet(Vector2 alpha, float epsilon, float r)
    : Entity2D("NodeBullet", alpha), theta(r) {
  targetLifetime = epsilon;
  lifetime = 0;
  addTag(tag);
}

NodeBullet::NodeBullet(Vector2 alpha, Vector2 beta)
    : Entity2D("NodeBullet", alpha) {
  targetLifetime = Vector2Distance(alpha, beta) / speed;
  theta = atan2f(beta.y - alpha.y, beta.x - alpha.x);
  lifetime = 0;
  addTag(tag);
}

NodeBullet::~NodeBullet() {}

void NodeBullet::render() {
  // draw a rectangle according to our dimensions and
  DrawCircleV(position, radius, color);
}

void NodeBullet::process(float delta) {
  lifetime += delta;
  position +=
      (Vector2) {delta * speed * cos(theta), delta * speed * sin(theta)};
  if(lifetime >= targetLifetime)
    killDefered();
  Border::wrapEntity(this);
}

void NodeBullet::death() {
  getParent()->addChild(new AttackNode(position));
}

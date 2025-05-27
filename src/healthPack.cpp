#include "healthPack.hpp"
#include "player.hpp"
#include "engine/entity.hpp"
#include "engine/core.h"
#include <cmath>
#include <raylib.h>
#include <raymath.h>

const Color HealthPack::col = RED;
const float HealthPack::friction = 50;
const float HealthPack::decayRate = .9;
const float HealthPack::playerPickupRadius = 200;
Player* HealthPack::plr = nullptr;

HealthPack::HealthPack(Vector2 p, float h) : Entity2D("HealthPack", p), health(h), velCart({0, 0}), velPol(0), isAttracted(false) {
  setPlayer();
}
HealthPack::HealthPack(Vector2 p, Vector2 v, float h) : Entity2D("HealthPack", p), health(h), velCart(v), velPol(0), isAttracted(false) {
  setPlayer();
}
HealthPack::~HealthPack() {}

float HealthPack::getRadiusForHealth() {
  return std::sqrt(health) * 10;
}

bool HealthPack::getIsAttracted() { return isAttracted; }

void HealthPack::Render() {
  const float r = getRadiusForHealth();
  DrawCircleV(Position, r, col);
}

void HealthPack::setPlayer() {
  plr = (Player*)Engine::searchTreeForEntity(&getRoot()->Children, "Player");
}

void HealthPack::Process(float delta) {
  // ok so the way i do this is really weird, we have two methods of movement, cartesian and polar
  // cartesien is for when we're just chillin and moving about *before* we get picked up
  // the latter however is for when we're picked up, so that way it's a generally consistent time to be
  // picked up (also it looks really cool)
  if(plr == nullptr)
    return;
  const Vector2 offset = Vector2Subtract(plr->Position, Position);
  if (!isAttracted) {
    Position = Vector2Add(Position, Vector2Scale(velCart, delta));
    velCart = Vector2Scale(velCart, delta * friction);
    health = health - (health * decayRate * delta);
    if (health < 0)
      killDefered();
    if (Vector2Length(offset) <= playerPickupRadius) {
      // here is our swap to polar
      isAttracted = true;
      // bassically this is just getting the alignment to the plr (hence the dot)
      // and scaling it so that way we keep *some* of our velocity
      float dot = Vector2DotProduct(velCart, offset);
      float mag = Vector2Length(velCart) * dot;
      const float baseVel = 10;
      velPol = mag - baseVel;
    }
  } else {
    const float cutoffDistance = 50;
    plr->getHealthManager()->applyHealing(health);
    velPol += delta * 100;
    const Vector2 velToAdd = Vector2Scale(offset, velPol * delta);
    Position = Vector2Add(Position, velToAdd);
    if (Vector2Length(offset) <= cutoffDistance)
      killDefered();
  }
}

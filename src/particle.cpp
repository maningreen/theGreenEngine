#include "particle.hpp"
#include "engine/entity.hpp"
#include "border.hpp"
#include <cstdlib>
#include "include.h"

float Particle::MaxLifetime = .5;
float Particle::MinLifetime = .1;

void Particle::render() {
  //sick so here's what we wanna do here
  //draw a square!1!!!!
  DrawRectanglePro((Rectangle){position.x, position.y, length, length}, (Vector2){length / 2.0f, length / 2.0f}, rotation, colour);
}

void Particle::process(float delta) {
  position = Vector2Add(position, Vector2Scale(velocity, delta));
  rotation += rotationalVelocity * delta; //btw the * delta is because it's m/s^2 and so you have to account for that s^2 there
  lifetime += delta;
  Border::wrapEntity(this);
  if(lifetime > maxLifetime) {
    length += -length * 1 * delta;
    colour.a += -colour.a * 5 * delta;
  }
  if(length <= 1 || colour.a <= 10)
    killDefered();
}


Particle::Particle(Vector2 pos, Vector2 vel) : Entity2D("Particle", pos), velocity(vel) {
  //then we set our stuff randomly :thumbsup:
  rotation = 360.0f * (float)rand() / (float)RAND_MAX; //degrees cuz the raylib function are in degrees :/
  //add some variance to the velocity (don't want it being *not* random)
  velocity = Vector2Scale(vel, ((float)rand() / (float)RAND_MAX) / 2 + .5);
  //add some random direction to velocity
  float scalar = sqrtf(Vector2LengthSqr(velocity) / 2.0f);
  velocity = Vector2Add(velocity, (Vector2){scalar * ((float)rand() / (float)RAND_MAX) - scalar / 2.0f, scalar * ((float)rand() / (float)RAND_MAX) - scalar / 2.0f});
  rotationalVelocity = 360.0f * (float)rand() / (float)RAND_MAX; //TODO random vel
  length = (20 * (float)rand() / (float)RAND_MAX) - 10 + 20;
  lifetime = 0;
  colour = (Color){YELLOW.r, YELLOW.g, YELLOW.b, 150};
  maxLifetime = ((MaxLifetime - MinLifetime) * (float)rand() / (float)RAND_MAX) + MinLifetime;
}

float Particle::GetLifetime() {
  return lifetime;
}

Particle::~Particle() {}

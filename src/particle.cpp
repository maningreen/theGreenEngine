#include "particle.hpp"
#include "entity.hpp"
#include "border.hpp"
#include <cstdlib>
#include <raylib.h>
#include <raymath.h>

float Particle::MaxLifetime = 2;
float Particle::MinLifetime = 1;

void Particle::Render() {
  //sick so here's what we wanna do here
  //draw a square!1!!!!
  DrawRectanglePro((Rectangle){Position.x, Position.y, Length, Length}, (Vector2){Length / 2.0f, Length / 2.0f}, Rotation, Colour);
}

void Particle::Process(float delta) {
  Position = Vector2Add(Position, Vector2Scale(Velocity, delta));
  Rotation += RotationalVelocity * delta; //btw the * delta is because it's m/s^2 and so you have to account for that s^2 there
  lifetime += delta;
  wrapPosition();
  if(lifetime > maxLifetime)
    Colour.a += -Colour.a * 5 * delta;
}


Particle::Particle(Vector2 pos, Vector2 vel) : Entity2D("Particle", pos), Velocity(vel) {
  //then we set our stuff randomly :thumbsup:
  Rotation = 360.0f * (float)rand() / (float)RAND_MAX; //degrees cuz the raylib function are in degrees :/
  //add some variance to the velocity (don't want it being *not* random)
  Velocity = Vector2Scale(vel, ((float)rand() / (float)RAND_MAX) / 2 + .5);
  //add some random direction to velocity
  float scalar = sqrtf(Vector2LengthSqr(Velocity) / 2.0f);
  Velocity = Vector2Add(Velocity, (Vector2){scalar * ((float)rand() / (float)RAND_MAX) - scalar / 2.0f, scalar * ((float)rand() / (float)RAND_MAX) - scalar / 2.0f});
  RotationalVelocity = 360.0f * (float)rand() / (float)RAND_MAX; //TODO random vel
  Length = (20 * (float)rand() / (float)RAND_MAX) - 10 + 20;
  lifetime = 0;
  Colour = YELLOW;
  maxLifetime = ((MaxLifetime - MinLifetime) * (float)rand() / (float)RAND_MAX) + MinLifetime;
  printf("%f\n", maxLifetime);
}

void Particle::wrapPosition() {
  bool out = false;
  if(Position.x < -Border::Length)
    Position.x += Border::Length * 2;
  else if(Position.x > Border::Length)
    Position.x -= Border::Length * 2;
  if(Position.y < -Border::Length)
    Position.y += Border::Length * 2;
  else if(Position.y > Border::Length)
    Position.y -= Border::Length * 2;
}

float Particle::GetLifetime() {
  return lifetime;
}

Particle::~Particle() {}

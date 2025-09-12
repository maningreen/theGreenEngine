#include "player.hpp"
#include "afterimage.hpp"
#include "engine/core.h"
#include "bars.hpp"
#include "border.hpp"
#include "dashNode.hpp"
#include "enemy.hpp"
#include "engine/entity.hpp"
#include "healthManager.hpp"
#include "particle.hpp"
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <functional>
#include <raylib.h>
#include <raymath.h>
#include <string>

extern "C" {
  extern float getDistanceFromLineAndPoint(float aX, float aY, float bX, float bY, float cX, float cY);
};

#define max(a, b) (a < b ? b : a)
#define min(a, b) (a < b ? a : b)

int Player::upKey = KEY_W;
int Player::downKey = KEY_S;
int Player::leftKey = KEY_A;
int Player::rightKey = KEY_D;

int Player::dashKey = KEY_SPACE;
float Player::dashTime = .4;
float Player::dashSpeed = 2500;

float Player::maxHealth = 10;

const float Player::defaultSpeed = 4000;
const float Player::defaultFriction = 58;

float Player::dashControl = 2;
float Player::dashCooldown = 1.5;
float Player::dashRegenDelay = .7;
unsigned Player::maxDashCount = 3;

float Player::particleSpawnTime = 1.0f / 30.0f;

const float distance = 50;

float Player::hitboxRadius = 25;


#define barDimensions (Vector2){10, 100}

Vector2 Player::getInput() {
  return Player::getInput(Player::upKey, Player::downKey, Player::leftKey,
                          Player::rightKey);
}

Vector2 Player::getInput(int u, int d, int l, int r) {
  Vector2 out = (Vector2){(float)IsKeyDown(r) - (float)IsKeyDown(l),
                          (float)IsKeyDown(d) - (float)IsKeyDown(u)};
  return Vector2Normalize(out);
}

void Player::Render() {
  // get the mouse position (in cartesian)
  // draw our triangle
  DrawTriangle(Position,
               Vector2Add(Position,
                          (Vector2){cosf(Rotation + 4 * M_PI / 3) * distance,
                                    -sinf(Rotation + 4 * M_PI / 3) * distance}),
               Vector2Add(Position, (Vector2){cosf(Rotation) * distance,
                                              -sinf(Rotation) * distance}),
               YELLOW);
  DrawTriangle(Position,
               Vector2Add(Position, (Vector2){cosf(Rotation) * distance,
                                              -sinf(Rotation) * distance}),
               Vector2Add(Position,
                          (Vector2){cosf(Rotation + 2 * M_PI / 3) * distance,
                                    -sinf(Rotation + 2 * M_PI / 3) * distance}),
               YELLOW);

  // we draw them darn sqrs
  const float height = 10;
  const Vector2 dems = {dashCooldownBar->Dimensions.x, height};
  // maxDashCount is the amount we draw
  float offsetY = dashCooldownBar->Dimensions.y / maxDashCount;

  for (int i = 0; i < maxDashCount; i++) {
    DrawRectangleV((Vector2){dashCooldownBar->Position.x,
                             dashCooldownBar->Position.y + (i * offsetY)},
                   dems, dashCooldownBar->EmptyCol);
  }

  DrawCircleV(Position, 5, WHITE);

  manageAttack();
}

void Player::Process(float delta) {
  lifetime += delta;
  Vector2 inputDirection = getInput();
  Velocity = Vector2Add(Vector2Scale(inputDirection, delta * Speed), Velocity);
  Position = Vector2Add(Position, Vector2Scale(Velocity, delta));
  Velocity = Vector2Scale(Velocity, delta * Friction);

  Border::wrapEntity(this);

  if(dashing || Vector2LengthSqr(inputDirection))
    if(fmodf(lifetime - particleSpawnTime, particleSpawnTime) <= 1.0 / 60.0f)
      getRoot()->addChild(new Particle(
          Position,
          Vector2Scale(
              Vector2Add(Velocity, Vector2Scale(inputDirection, Speed * delta)),
              -1)));

  timeSinceDash += delta;
  if(dashing) {
    Velocity = dashDirection;
    // i wanna do something funny.
    // that funny is changing the dashDirection.
    // based on the playerInput.
    dashDirection = Vector2Scale(
        Vector2Normalize(Vector2Add(
            dashDirection,
            Vector2Scale(inputDirection,
                         (delta / dashTime) * dashSpeed * dashControl))),
        dashSpeed);

    if(fmodf(timeSinceDash, .1) < 1.0f / 120.0f)
      getRoot()->addChild(new Afterimage(Position, Rotation));

    if(timeSinceDash > dashTime)
      dashing = false;
  } else if(IsKeyPressed(dashKey) && dashProgress > 1) {
    dashing = true;
    dashProgress--;
    dashDirection = Vector2Length(inputDirection) > 0
                        ? Vector2Scale(inputDirection, dashSpeed)
                        : Vector2Scale(Vector2Normalize(Velocity), dashSpeed);
    timeSinceDash = 0;
    addChild(new DashNode(Position));
  } else if(dashProgress <= maxDashCount && timeSinceDash > dashRegenDelay)
    dashProgress += delta / dashCooldown;

  if(healthManager->isDead())
    killDefered();

  manageBars();
  manageRotation();
}

void Player::manageBars() {
  manageBar(dashCooldownBar, 1, dashProgress / maxDashCount, true);
}

void Player::manageBar(Bar *b, int index, float p, bool shouldRender) {
  // so basically, math :thumbsup:
  // bool for verticle b->growVert
  float offsetX =
      /*this one is either constant or index dependant depending on growVert*/
          !b->ShrinkY
          ? -b->Dimensions.x / 2.0f
          : distance + b->Dimensions.x * index;
  float offsetY = /*just the opposite of ^*/ b->ShrinkY
                      ? -b->Dimensions.y / 2.0f
                      : distance + b->Dimensions.y * index;
  Vector2 finalPosition = {Position.x + offsetX, Position.y + offsetY};
  b->Position = finalPosition;
  b->ShouldRender = shouldRender;
  b->Progress = p;
}

void Player::manageRotation() {
  Vector2 mousePos =
      Vector2Scale(Vector2Subtract(GetMousePosition(), cam->Camera.offset),
                   1.0f / cam->Camera.zoom);
  // then we also have to globalize the mouse position good thing we have a cam
  // field
  mousePos = Vector2Add(mousePos, cam->Camera.target);
  Rotation = atan2f(-(mousePos.y - Position.y),
                    mousePos.x - Position.x); // then it's as simple as b - a
}

Player::Player(const std::string &name, Vector2 position, CameraEntity *camera)
    : Entity2D(name, position), cam(camera) {

  healthManager = new HealthManager(
      maxHealth,
      BarManager(&Position, distance,
                 Bar(Position, (Vector2){barDimensions.y, barDimensions.x}, RED,
                     DARKGRAY, false)));
  addChild(healthManager);

  Velocity = (Vector2){0, 0};
  Speed = defaultSpeed;
  Friction = defaultFriction;

  dashDirection = Vector2Zero();
  dashing = false;
  dashProgress = 0;
  timeSinceDash = 0;

  dashCooldownBar =
      new Bar(Position, barDimensions, YELLOW, (Color){10, 10, 10, 255}, true);
  addChild(dashCooldownBar);
  Enemy::setPlayer();
}

Player::~Player() {
  Enemy::setPlayer(nullptr);
  cam->Follow = nullptr;
}

void Player::Init() { Enemy::setPlayer(); };

void Player::setCam(CameraEntity *camra) { cam = camra; }

float Player::getLifetime() { return lifetime; }

HealthManager *Player::getHealthManager() { return healthManager; }

bool Player::getDashing() { return dashing; }

void Player::manageAttack() {
  // i am going to use a lot of lambdas here, cuz i just learned that they exist in c++ and i just wrote a ton of haskell

  // if we don't have enough to attack, we don't manage the attack
  std::vector<DashNode*> nodes = DashNode::getNodes();

  if(nodes.size() < 3) return;

  //get if it's a regular triangle
  float theta = 0;
  for(DashNode* n : nodes) {
    float a = n->getInternalAngle();
    printf("%f\n", a);
    theta += a;
  }
  float area;
  if(theta - 180 < 1) { // ITS A REGULAR TRIGLE!
    // we wanna figure out the area
    // which is bh/2
    // base, easy peasy, lemon squeazy
    float b = Border::getDistance(nodes[0]->Position, nodes[1]->Position);
    // height?... transformation matrix... :(
    // lets say node[0] to node[1] is the base, and node[2] is gonna be the rest
    // we can assume node[0]->getLasAngle() will have the angle to node[1]
    float theta = nodes[0]->getLasAngle();
    Vector2 shortestVector = Border::getShortestPathToPoint(nodes[0]->Position, nodes[2]->Position);
    Vector2 localVectorToPeak = {
      .x = shortestVector.x * cos(theta) + shortestVector.y * sin(theta),
      .y = -shortestVector.x * sin(theta) + shortestVector.y * cos(theta),
    };
    float h = localVectorToPeak.y;
    // sick, then we just do bh/2
    area = b * h / 2;
    return;
  } else
    area = 0;

  // sort the nodes via ~~magic~~ distance
  std::sort(nodes.begin(), nodes.end(), [this](DashNode* a, DashNode* b){
    return Vector2DistanceSqr(Position, a->Position) < Vector2DistanceSqr(Position, b->Position);
  });

  // get m
  float m = (nodes.front()->Position.y - nodes.back()->Position.y) / (nodes.front()->Position.x - nodes.back()->Position.x);
  float mInverse = -1.0f / m;

  // LINEAR ALGEBRA, this function is to be mapped to the vector
  // i wish i could curry, then this wouldn't be an issue, but this is c++; and not haskell :(
  auto f = [this, nodes, mInverse, m, area](Enemy* en, std::function<float(float, float, Entity2D*, Entity2D*)> g, Entity2D* a, Entity2D* b){
    // check if this equation will even work
    Vector2 endP;
    if(mInverse == 0) { // if this is true then m is zero, and everything explodes :)
      if(nodes.front()->Position.y == nodes.back()->Position.y) // this means that we can confirm the endP.y is en's y
        endP = {
          .x = nodes.front()->Position.x,
          .y = en->Position.y
        };
      else // this means that we can confirm the endP.x is en's x
        endP = {
          .x = en->Position.x,
          .y = nodes.front()->Position.y
        };
    } else {
      float x = (nodes.front()->Position.x + mInverse * (mInverse * en->Position.x - en->Position.y + nodes.front()->Position.y)) / (mInverse * mInverse + 1); // i don't wanna talk abt it :(
      endP = {
        .x = x,
        .y = g(m, x, nodes.front(), nodes.back())
      };
    }
    // SICK
    // we now have endP
    // all we have to do now is check distance :)
    return Vector2Distance(endP, en->Position);
  };

  std::vector<Entity*> enemies = Engine::getAllChildrenWithTagRecursive(getRoot(), "Enemy");

  for(Entity* en : enemies) {
    // we figure out the function for the actual slope thingy
    if(area == 0) {
      float minDist = 100000000000000000000.0f;
      for(int i = 0; i < 3; i++) {
        float d = getDistanceFromLineAndPoint(nodes[i]->Position.x, nodes[i]->Position.y, 
                                              nodes[i]->getNext()->Position.x, nodes[i]->getNext()->Position.y, 
                                              ((Entity2D*)en)->Position.x, ((Entity2D*)en)->Position.y);
        minDist = minDist > d ? d : minDist;
      }
      if(minDist <= ((Enemy*)en)->Radius && area == 0)
        en->killDefered();
    } else {
      int i = DashNode::getBreakInPolygon();
      if(i == -1) {
        // then we can use a regular collision
        // checking collision circle triangle, easy peasy, lemon squezy
        Vector2 avg = Vector2Zero();
        for(int i = 0; i < 3; i++)
          avg = Vector2Add(avg, nodes[i]->Position);
        avg = Vector2Scale(avg, 1.0f / 3.0f);

        Vector2 vecToAvg = Vector2Subtract(avg, ((Enemy*)en)->Position);
        float dist = Vector2Length(vecToAvg);
        float r = ((Enemy*)en)->Radius;

        float min = dist < r ? r : dist;
        Vector2 p = Vector2Add(Position, Vector2Scale(vecToAvg, dist / min));
        if(CheckCollisionPointTriangle(p, nodes[0]->Position, nodes[1]->Position, nodes[2]->Position))
          ((Enemy*)en)->getHealthManager()->applyDamage(1.0f / area);
      } 
    }
  }
}

#include "player.hpp"
#include "afterimage.hpp"
#include "bars.hpp"
#include "border.hpp"
#include "dashNode.hpp"
#include "enemy.hpp"
#include "engine/core.h"
#include "engine/entity.hpp"
#include "healthManager.hpp"
#include "particle.hpp"
#include <algorithm>
#include <cmath>
#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <string>

extern "C" {
  float getDistanceFromLineAndPoint(float aX, float aY, float bX, float bY, float cX, float cY);
  void getClosestPointFromLineAndPoint(Vector2* a, Vector2* b, Vector2* c, Vector2* out);
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
  return getInputVector(
    Player::upKey, 
    Player::downKey, 
    Player::leftKey,
    Player::rightKey);
}

void Player::Render() {
  // get the mouse position (in cartesian)
  // draw our triangle
  DrawTriangle(
    Position,
    Position + (Vector2){cosf(rotation + 4 * PI / 3) * distance,
                        -sinf(rotation + 4 * PI / 3) * distance},
    Position + (Vector2){cosf(rotation) * distance,
                        -sinf(rotation) * distance},
    YELLOW);
  DrawTriangle(
    Position,
    Position + (Vector2){cosf(rotation) * distance,
                          -sinf(rotation) * distance},
    Position + (Vector2){cosf(rotation + 2 * PI / 3) * distance,
                         -sinf(rotation + 2 * PI / 3) * distance},
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
  velocity = inputDirection * delta * speed + velocity;
  Position = Position + velocity * delta;
  velocity = velocity * delta * friction;

  Border::wrapEntity(this);

  if (dashing || Vector2LengthSqr(inputDirection))
    if (fmodf(lifetime - particleSpawnTime, particleSpawnTime) <= 1.0 / 60.0f)
      getRoot()->addChild(new Particle(
          Position,
          Vector2Scale(
              Vector2Add(velocity, Vector2Scale(inputDirection, speed * delta)),
              -1)));

  timeSinceDash += delta;
  if (dashing) {
    velocity = dashDirection;
    // i wanna do something funny.
    // that funny is changing the dashDirection.
    // based on the playerInput.
    dashDirection = Vector2Scale(
        Vector2Normalize(Vector2Add(
            dashDirection,
            Vector2Scale(inputDirection,
                         (delta / dashTime) * dashSpeed * dashControl))),
        dashSpeed);

    if (fmodf(timeSinceDash, .1) < 1.0f / 120.0f)
      getRoot()->addChild(new Afterimage(Position, rotation));

    if (timeSinceDash > dashTime)
      dashing = false;
  } else if (IsKeyPressed(dashKey) && dashProgress > 1) {
    dashing = true;
    dashProgress--;
    dashDirection = Vector2Length(inputDirection) > 0
                        ? Vector2Scale(inputDirection, dashSpeed)
                        : Vector2Scale(Vector2Normalize(velocity), dashSpeed);
    timeSinceDash = 0;
    addChild(new DashNode(Position));
  } else if (dashProgress <= maxDashCount && timeSinceDash > dashRegenDelay)
    dashProgress += delta / dashCooldown;

  if (healthManager->isDead())
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
      !b->ShrinkY ? -b->Dimensions.x / 2.0f
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
  rotation = atan2f(-(mousePos.y - Position.y),
                    mousePos.x - Position.x); // then it's as simple as b - a
}

Player::Player(const std::string &name, Vector2 position, CameraEntity *camera)
    : Entity2D(name, position), cam(camera) {

  healthManager = new HealthManager(
    maxHealth,
    BarManager(&Position, distance,
      Bar(Position, 
        (Vector2){barDimensions.y, barDimensions.x}, 
          RED,
          DARKGRAY, 
          false)));
  addChild(healthManager);

  velocity = (Vector2){0, 0};
  speed = defaultSpeed;
  friction = defaultFriction;

  dashDirection = Vector2Zero();
  dashing = false;
  dashProgress = 0;
  timeSinceDash = 0;

  dashCooldownBar =
      new Bar(Position, barDimensions, YELLOW, (Color){10, 10, 10, 255}, true);
  addChild(dashCooldownBar);

  cam = new CameraEntity("Camera", this);
  addChild(cam);

  Enemy::setPlayer();
}

Player::~Player() {
  Enemy::setPlayer(nullptr);
}

void Player::Init() { Enemy::setPlayer(); };

float Player::getLifetime() { return lifetime; }

HealthManager *Player::getHealthManager() { return healthManager; }

bool Player::getDashing() { return dashing; }

void Player::manageAttack() {
  // i am going to use a lot of lambdas here, cuz i just learned that they exist
  // in c++ and i just wrote a ton of haskell

  // if we don't have enough to attack, we don't manage the attack
  std::vector<DashNode *> nodes = DashNode::getNodes();

  if (nodes.size() < 3)
    return;

  // get if it's a regular triangle
  float theta = 0;
  for (DashNode *n : nodes)
    theta += abs(n->getInternalAngle());
  float area;
  // printf("theta: %f\n", theta * RAD2DEG);
  if (abs(theta - PI) < DEG2RAD) { // ITS A REGULAR TRIGLE!
    // we wanna figure out the area
    // which is bh/2
    // base, easy peasy, lemon squeazy
    float b = Border::getDistance(nodes[0]->Position, nodes[1]->Position);
    // height?... transformation matrix... :(
    // lets say node[0] to node[1] is the base, and node[2] is gonna be the rest
    // we can assume node[0]->getLasAngle() will have the angle to node[1]
    float theta = nodes[0]->getLasAngle();
    Vector2 shortestVector =
        Border::getShortestPathToPoint(nodes[0]->Position, nodes[2]->Position);
    Vector2 localVectorToPeak = {
        .x = shortestVector.x * cos(theta) + shortestVector.y * sin(theta),
        .y = -shortestVector.x * sin(theta) + shortestVector.y * cos(theta),
    };
    float h = localVectorToPeak.y;
    // sick, then we just do bh/2
    area = abs(b * h / 2); // sometimes it goes negative :p
  } else
    area = 0;

  // sort the nodes via ~~magic~~ distance
  std::sort(nodes.begin(), nodes.end(), [this](DashNode *a, DashNode *b) {
    return Vector2DistanceSqr(Position, a->Position) <
           Vector2DistanceSqr(Position, b->Position);
  });

  std::vector<Entity *> enemies =
      Engine::getAllChildrenWithTagRecursive(getRoot(), "Enemy");

  for (Entity *enButEnt : enemies) {
    Enemy *en = (Enemy *)enButEnt;
    // we figure out the function for the actual slope thingy
    // printf("area: %f\n", area);
    if (area == 0) {
      float minDist = INFINITY;
      // printf("Looping over all items\n");
      for (int i = 0; i < 3; i++) {
        DashNode unwrappedNext = nodes[i]->getNext()->unwrapRelative();
        Vector2 closestPoint;
        // DrawLineEx(Vector2Lerp(nodes[i]->Position, unwrappedNext.Position, -10), unwrappedNext.Position, 20, GREEN);
        getClosestPointFromLineAndPoint(&nodes[i]->Position, &unwrappedNext.Position, &en->Position, &closestPoint);
        float d = Vector2Distance(closestPoint, en->Position);
        minDist = min(d, minDist);
        DrawCircleV(closestPoint, 30, PURPLE);
      }
      // DrawCircleV(*x, 10, BLUE);
      // free(x);
      // if (minDist != 100000000000000000000.0f)
        // DrawCircleV(en->Position, minDist, GREEN);
      if (minDist <= en->Radius)
        en->killDefered();
    } else {
      // we got a break so then we should handle that break by schooching it
      // over to the other side :) we're gonna use quadrants for this tomfoolery
      // (relative to other two)
      // -x, -y -> what do?
      // (True, False) -> reflect x
      // (False, True) -> reflect y
      // (True, True) -> reflect x, reflect x
      // but um what if there are two breakers?
      // we can use this loop-ily (idk the term)
      std::vector<DashNode> effectiveNodes;
      for (DashNode *n : nodes)
        effectiveNodes.push_back(n->unwrapRelative());
      // then we can use a regular collision
      // checking collision circle triangle, easy peasy, lemon squezy
      Vector2 avg = Vector2Zero();
      for (int i = 0; i < 3; i++)
        avg = Vector2Add(avg, effectiveNodes[i].Position);
      avg = Vector2Scale(avg, 1.0f / 3.0f);

      Vector2 vecToAvg = Vector2Subtract(avg, en->Position);
      float dist = Vector2Length(vecToAvg);
      float r = en->Radius;

      float min = dist < r ? dist : r;
      Vector2 p = Vector2Add(en->Position, Vector2Scale(vecToAvg, min / dist));
      if (CheckCollisionPointTriangle(p, effectiveNodes[0].Position,
                                      effectiveNodes[1].Position,
                                      effectiveNodes[2].Position))
        ((Enemy *)en)->getHealthManager()->applyDamage(.5);
    }
  }
}

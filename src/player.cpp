#include "player.hpp"
#include "afterimage.hpp"
#include "bars.hpp"
#include "border.hpp"
#include "attackNode.hpp"
#include "enemy.hpp"
#include "engine/core.h"
#include "engine/entity.hpp"
#include "healthManager.hpp"
#include "particle.hpp"
#include "raylib.h"
#include <algorithm>
#include <cmath>
#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <string>

extern "C" {
float getDistanceFromLineAndPoint(float aX, float aY, float bX, float bY,
    float cX, float cY);
void getClosestPointFromLineAndPoint(Vector2* a, Vector2* b, Vector2* c,
    Vector2* out);
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

#define barDimensions (Vector2) {10, 100}

Vector2 Player::getInput() {
  return getInputVector(Player::upKey,
      Player::downKey,
      Player::leftKey,
      Player::rightKey);
}

void Player::Render() {
  // get the mouse position (in cartesian)
  // draw our triangle
  DrawTriangle(Position,
      Position + (Vector2) {cosf(rotation + 4 * PI / 3) * distance,
                     -sinf(rotation + 4 * PI / 3) * distance},
      Position +
          (Vector2) {cosf(rotation) * distance, -sinf(rotation) * distance},
      YELLOW);
  DrawTriangle(Position,
      Position +
          (Vector2) {cosf(rotation) * distance, -sinf(rotation) * distance},
      Position + (Vector2) {cosf(rotation + 2 * PI / 3) * distance,
                     -sinf(rotation + 2 * PI / 3) * distance},
      YELLOW);

  // we draw them darn sqrs
  const float height = 10;
  const Vector2 dems = {dashCooldownBar->Dimensions.x, height};
  // maxDashCount is the amount we draw
  float offsetY = dashCooldownBar->Dimensions.y / maxDashCount;

  for(int i = 0; i < maxDashCount; i++) {
    DrawRectangleV((Vector2) {dashCooldownBar->Position.x,
                       dashCooldownBar->Position.y + (i * offsetY)},
        dems,
        dashCooldownBar->EmptyCol);
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

  if(dashing || Vector2LengthSqr(inputDirection))
    if(fmodf(lifetime - particleSpawnTime, particleSpawnTime) <= 1.0 / 60.0f)
      getRoot()->addChild(new Particle(Position,
          Vector2Scale(
              Vector2Add(velocity, Vector2Scale(inputDirection, speed * delta)),
              -1)));

  timeSinceDash += delta;
  if(dashing) {
    velocity = dashDirection;
    // i wanna do something funny.
    // that funny is changing the dashDirection.
    // based on the playerInput.
    dashDirection =
        Vector2Scale(Vector2Normalize(Vector2Add(dashDirection,
                         Vector2Scale(inputDirection,
                             (delta / dashTime) * dashSpeed * dashControl))),
            dashSpeed);

    if(fmodf(timeSinceDash, .1) < 1.0f / 120.0f)
      getRoot()->addChild(new Afterimage(Position, rotation));

    if(timeSinceDash > dashTime)
      dashing = false;
  } else if(IsKeyPressed(dashKey) && dashProgress > 1) {
    dashing = true;
    dashProgress--;
    dashDirection = Vector2Length(inputDirection) > 0
                        ? Vector2Scale(inputDirection, dashSpeed)
                        : Vector2Scale(Vector2Normalize(velocity), dashSpeed);
    timeSinceDash = 0;
    if(AttackNode::getNodes().size() < 3)
      addChild(new AttackNode(Position));
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

void Player::manageBar(Bar* b, int index, float p, bool shouldRender) {
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

Player::Player(const std::string& name, Vector2 position, CameraEntity* camera)
    : Entity2D(name, position), cam(camera) {

  healthManager = new HealthManager(maxHealth,
      BarManager(&Position,
          distance,
          Bar(Position,
              (Vector2) {barDimensions.y, barDimensions.x},
              RED,
              DARKGRAY,
              false)));
  addChild(healthManager);

  velocity = (Vector2) {0, 0};
  speed = defaultSpeed;
  friction = defaultFriction;

  dashDirection = Vector2Zero();
  dashing = false;
  dashProgress = 0;
  timeSinceDash = 0;

  dashCooldownBar =
      new Bar(Position, barDimensions, YELLOW, (Color) {10, 10, 10, 255}, true);
  addChild(dashCooldownBar);

  cam = new CameraEntity("Camera", this);
  addChild(cam);

}

Player::~Player() { 
  Enemy::setPlayer(nullptr);
}

void Player::Init() { Enemy::setPlayer(); };

float Player::getLifetime() { return lifetime; }

HealthManager* Player::getHealthManager() { return healthManager; }

bool Player::getDashing() { return dashing; }

bool getTriangleIsValid(float angleSum) {
  return angleSum - PI < DEG2RAD;
}

// calculates the area of a triangle with three *unwrapped* coordinates
float calculateTriangleArea(Vector2 a, Vector2 b, Vector2 c) {

  // formula for area of a triangle b * h / 2
  float bLength = Border::getDistance(a, b);

  float theta = atan2(b.y - a.y, b.x - a.x); // getting theta for the operation...
  Vector2 shortestVector = c - a;

  // we apply a transformation matrix, but only keep the y, as that's the relative height
  float triangleHeight = -shortestVector.x * sin(theta) + shortestVector.y * cos(theta);

  return bLength * triangleHeight / 2;
}

void Player::manageAttack() {
  std::vector<AttackNode*> nodes = AttackNode::getNodes();

  if(nodes.size() < 3) // we can't make a polygon with less than 3 vertices
    return;

  float theta = 0;
  for(AttackNode* n: nodes)
    theta += abs(n->getInternalAngle());

  float area;
  if(getTriangleIsValid(theta))
    area = calculateTriangleArea(nodes[0]->Position, nodes[1]->Position, nodes[2]->Position);
  else
    area = 0;

  // sort the nodes via ~~magic~~ distance
  std::sort(nodes.begin(), nodes.end(), [this](AttackNode* a, AttackNode* b) {
    return Vector2DistanceSqr(Position, a->Position) <
           Vector2DistanceSqr(Position, b->Position);
  });

  std::vector<Entity*> enemies =
      Engine::getAllChildrenWithTagRecursive(getRoot(), "Enemy");

  for(Entity* enButEnt: enemies) {
    Enemy* en = (Enemy*)enButEnt;
    // we figure out the function for the actual slope thingy
    if(area == 0) {
      float minDist = INFINITY;
      for(int i = 0; i < 3; i++) {

        Vector2 unwrappedNext = nodes[i]->getNext()->unwrapRelative().Position;
        Vector2 closestPoint;

        // this is an external function written in haskell
        getClosestPointFromLineAndPoint(&nodes[i]->Position,
            &unwrappedNext,
            &en->Position,
            &closestPoint);
        float d = Vector2Distance(closestPoint, en->Position);
        minDist = min(d, minDist);
      }
      if(minDist <= en->Radius)
        en->killDefered();
    } else {
      // so in this the triangle is regular, so we want to treat it like one; however, sometimes it needs to be unwrapped, otherwise it's considered having a much larger
      // area than it should

      Vector2 effectivePos[nodes.size()];
      for(int i = 0; i < nodes.size(); i++)
        effectivePos[i] = Border::unwrapPositionRelative(en->Position, nodes[i]->Position);

      // then we can use a regular collision
      // checking collision circle triangle, easy peasy, lemon squeazy
      Vector2 avg = Vector2Zero();
      for(int i = 0; i < 3; i++)
        avg = Vector2Add(avg, effectivePos[i]);
      avg = Vector2Scale(avg, 1.0f / 3.0f);


      Vector2 vecToAvg = Vector2Subtract(avg, en->Position);
      float dist = Vector2Length(vecToAvg);
      float r = en->Radius;

      float min = dist < r ? dist : r;
      Vector2 p = Vector2Add(en->Position, Vector2Scale(vecToAvg, min / dist));
      if(CheckCollisionPointTriangle(p,
             effectivePos[0],
             effectivePos[1],
             effectivePos[2]))
        ((Enemy*)en)->getHealthManager()->applyDamage( 60000 / area);
    }
  }
}

#include "attackNode.hpp"

#include <cmath>
#include <vector>

#include "border.hpp"
#include "engine/entity.hpp"
#include "engine/world.hpp"
#include "include.h"
#include "laser.hpp"
#include "player.hpp"
#include "raylib.h"
#include "raymath.h"

#define min(a, b) (a < b ? a : b)

float AttackNode::defaultRadius = 30;
float AttackNode::lifetimeAfterAttack = 2.5;
std::vector<AttackNode*> AttackNode::nodes;

float AttackNode::damage = 600;

AttackNode::AttackNode(Vector2 p)
  : Entity2D("DashNode", p), lifetime(0), radius(defaultRadius), las(position, 0, 500, WHITE) {
    las.shouldRender = false;
    index = nodes.size();
    nodes.push_back(this);
    addTag(Tags::nodeBullet);
}

AttackNode::AttackNode(Vector2 p, bool x)
  : Entity2D("DashNode", p), lifetime(0), radius(defaultRadius), las(position, 0, 500, WHITE) {
    las.shouldRender = false;
    addTag(Tags::nodeBullet);
}

AttackNode::~AttackNode() {
    for(int i = 0; i < nodes.size(); i++)
        if(nodes[i] == this) nodes.erase(nodes.begin() + i);
}

float AttackNode::ease(float x) {
    return 1 - (x * x * x * x);
}

int AttackNode::getIndex() {
    for(int i = 0; i < nodes.size(); i++)
        if(nodes[i] == this) return i;
    return -1;
}

float AttackNode::getMaxLifetime() {
    return 3.5 * Player::get().getDashManager()->regenRate;
}

std::vector<AttackNode*> AttackNode::getNodes() {
    return nodes;
}

AttackNode* AttackNode::getPrev() {
    return nodes[((index + nodes.size() - 1) % nodes.size())];
}

AttackNode* AttackNode::getNext() {
    return nodes[((index + 1) % nodes.size())];
}

float AttackNode::getLasAngle() {
    return las.rotation;
}

bool AttackNode::getBreakInLas() {
    return las.getBreaks();
}

float AttackNode::getInternalAngle() {
    if(nodes.size() < 3) return 0;
    float thetaA = getLasAngle();
    float thetaB = fmodf(getPrev()->getLasAngle() + PI, PI * 2);
    float deltaTheta = fmodf(thetaB - thetaA, PI * 2);
    if(deltaTheta < PI)
        return deltaTheta;
    else
        return 2 * PI - deltaTheta;
}

float AttackNode::getInternalAngleSum() {
    if(nodes.size() < 3) return -1;
    float theta = 0;
    for(int i = 0; i < nodes.size(); i++) theta += nodes[i]->getInternalAngle();
    return theta;
}

int AttackNode::getBreakInPolygon() {
    // THERE IS A SINGULAR EDGE CASE
    // that's if the first index is the one that's breaking it
    if(nodes[0]->getPrev()->getBreakInLas()) return 0;
    for(int i = 0; i < nodes.size(); i++)
        if(nodes[i]->getBreakInLas() && nodes[i]->getNext()->getBreakInLas()) return i + 1;
    return -1;
}

AttackNode AttackNode::unwrapRelative() {
    return Border::unwrapPositionRelative(this->position, getNext()->position);
}

float AttackNode::getPerimeter() {
    float sigma = 0;  // sigma as in the greek letter for summation, not brainrot
    for(int i = 1; i < nodes.size(); i++)
        sigma +=
          Vector2Length(Border::getShortestPathToPoint(nodes[i - 1]->position, nodes[i]->position));
    ;
    return sigma;
}

bool AttackNode::getTriangleIsRegular() {
    float angleSum = getInternalAngleSum();
    return getTriangleIsValid(angleSum);
}

bool AttackNode::getTriangleIsRegular(float angleSum) {
    return getTriangleIsValid(angleSum);
}

void AttackNode::render() {
    las.render();
    DrawCircleV(position, radius, WHITE);
    if(index == 0) manageAttack();
}

void AttackNode::process(float delta) {
    lifetime += delta;

    las.process(delta);

    if(nodes.size() >= 3) {
        AttackNode* next = getNext();
        Vector2 vectorToNext = Border::getShortestPathToPoint(this, next->position);
        if(!las.shouldRender) {
            las.shouldRender = true;
            lifetime = Player::get().getDashManager()->regenRate + -lifetimeAfterAttack;
            las.lookAt(next->position);
            radius = defaultRadius;
        }
        las.length = Vector2Length(vectorToNext) - (next->radius + radius);
        las.width = sqrt(radius);
        // then we wanna ummm crap, offset the umm mfrickennnaaahahhmmmm the
        // las->position yeah
        las.position = position + Vector2Scale(Vector2Normalize(vectorToNext), radius);
    } else if(lifetime <= 1)
        radius = ease(1 - lifetime) * defaultRadius;
    if(lifetime >= getMaxLifetime()) radius = ease(lifetime - getMaxLifetime()) * defaultRadius;

    // the +1 accounts for the time to ease out
    if(lifetime >= getMaxLifetime() + 1) killDefered();
}

void AttackNode::manageAttack() {
    if(nodes.size() < 3) return;

    bool validTriangle = getTriangleIsRegular();

    std::vector<unsigned> enemies = World::getAllEntitiesWithTag(Enemy::tag);

    for(int i = 0; i < enemies.size(); i++) {
        Enemy* en = (Enemy*)World::getEntity(enemies[i]);
        // we figure out the function for the actual slope thingy
        if(validTriangle) {
            // so in this the triangle is regular, so we want to treat it like one;
            // however, sometimes it needs to be unwrapped, otherwise it's considered
            // having a much larger area than it should

            // figure out which node is closest to this john
            int closestI;
            {
                float minDist = INFINITY;
                for(int i = 0; i < nodes.size(); i++) {
                    float d = Vector2DistanceSqr(nodes[i]->position, en->position);
                    if(minDist > d) {
                        minDist = d;
                        closestI = i;
                    }
                }
            }

            Vector2 effectivePos[nodes.size()];
            for(int i = 0; i < nodes.size(); i++) {
                if(i == closestI) {
                    effectivePos[i] = nodes[i]->position;
                    continue;
                }
                effectivePos[i] =
                  Border::unwrapPositionRelative(nodes[closestI]->position, nodes[i]->position);
            }

            // then we can use a regular collision
            // checking collision circle triangle, easy peasy, lemon squeezy
            Vector2 avg = Vector2Zero();
            for(int i = 0; i < 3; i++) avg = Vector2Add(avg, effectivePos[i]);
            avg = Vector2Scale(avg, 1.0f / 3.0f);

            Vector2 vecToAvg = Vector2Subtract(avg, en->position);
            float dist = Vector2Length(vecToAvg);
            float r = en->radius;

            float min = dist < r ? dist : r;
            Vector2 p = en->position + Vector2Scale(vecToAvg, min / dist);
            if(CheckCollisionPointTriangle(p, effectivePos[0], effectivePos[1], effectivePos[2]))
                en->getHealthManager().applyDamage(damage / getPerimeter());
        } else {
            float minimumDistance = INFINITY;
            for(int i = 0; i < 3; i++) {
            }
            if(minimumDistance <= en->radius)
                en->getHealthManager().applyDamage(damage / getPerimeter());
        }
    }
}

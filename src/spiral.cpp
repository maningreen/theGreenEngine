#include "spiral.hpp"
#include "enemy.hpp"
#include "include.h"
#include "enemyBullet.hpp"
#include <cmath>
#include <iostream>

Color Spiraler::Colour = BLUE;
float Spiraler::SpinLength = 10;
float Spiraler::SpinSpeed = 10;
float Spiraler::DefaultRadius = 50;

Spiraler::Spiraler() : Enemy(Vector2Zero()), isSpinning(true) {
  Radius = DefaultRadius;
}

void Spiraler::Render() {
  DrawCircleV(Position, Radius, Colour);
}

void Spiraler::Process(float delta) {
  manageHealthBar(Radius);
  spinTime += delta;
  addChild(new EnemyBullet(Position, spinTime * spinTime * 5, Colour));
}

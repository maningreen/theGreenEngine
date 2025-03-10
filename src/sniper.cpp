#include "sniper.hpp"
#include "enemy.hpp"

float Sniper::minDist = 1000;
float Sniper::maxDist = 1500;
float Sniper::rotationSpeed = M_PI / 2.0f;
Color Sniper::defaultColour = PURPLE;

Sniper::Sniper(Vector2 pos) : Enemy(pos), rotation(0) {
  setState(positioning);
}

Sniper::~Sniper() {}

//gur getters :p
//static
float Sniper::getMinDist() { return minDist; }
float Sniper::getMaxDist() { return maxDist; }
float Sniper::getRotationSpeed() { return rotationSpeed; }
Color Sniper::getDefaultColour() { return defaultColour; }
//regular
float Sniper::getRotation() const { return rotation; }
float Sniper::getStateTime() const { return stateTime; }

void Sniper::Process(float delta) {
  //we do state shtuff
  stateTime += delta;
}

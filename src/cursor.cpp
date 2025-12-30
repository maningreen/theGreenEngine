#include "cursor.hpp"
#include "player.hpp"
#include "border.hpp"

float Cursor::thickness = 10;
float Cursor::length = 30;
float Cursor::offset = 30;
float Cursor::centerRadius = 10;
float Cursor::offsetTheta = PI / 4;
float Cursor::waveSpeed = PI;
float Cursor::waveAmplitude = 10;
unsigned Cursor::markCount = 4;

Cursor::Cursor() : Entity2D("cursor", GetMousePosition()) {
  HideCursor();
  sigmaDelta = 0;
}

Cursor::~Cursor() {
  ShowCursor();
}

void Cursor::process(float delta) {
  sigmaDelta += delta;
  position = Border::wrapPos(Player::player->getCamera()->getMousePosition());
}

void Cursor::render() {
  for(int i = 0; i < markCount; i++) {
    // we draw little rectangles :3
    // but first circle
    float theta = i * 2 * PI / (float)markCount + offsetTheta;
    DrawRectanglePro(
      (Rectangle){
        cos(theta) * (offset + (1 + cos(sigmaDelta * waveSpeed) / 2) * waveAmplitude) + position.x,
        sin(theta) * (offset + (1 + cos(sigmaDelta * waveSpeed) / 2) * waveAmplitude) + position.y,
        length,
        thickness,
      }, 
      (Vector2){
        0,
        thickness / 2
      },
      theta * RAD2DEG, // degrees :(
      WHITE
    );
  }
  DrawCircleV(position, centerRadius, WHITE);
}

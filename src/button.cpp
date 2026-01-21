#include "button.hpp"
#include "player.hpp"
#include <raylib.h>

float Button::defaultPadding = 30;

#define SPACING 30

Button::Button(Vector2 position, std::string t, std::function<void(Button&)> call): Entity2D("button", position) {
  setText(t);
  padding = defaultPadding;
  callback = call;
}

void Button::setText(std::string i) {
  text = i;
  textDems = MeasureTextEx(GetFontDefault(), i.c_str(), 500, SPACING);
}

const std::string& Button::getText() {
  return text;
}

void Button::process(float delta) {
  if(Player::player == nullptr)
    return;
  Vector2 mousePos = Player::player->getCamera().getMousePosition();

  if(
    CheckCollisionPointRec(
      mousePos,
      (Rectangle){
        position.x,
        position.y,
        textDems.x,
        textDems.y
      }
    ) &&
    IsMouseButtonReleased(MOUSE_BUTTON_LEFT)
  ) {
    callback(*this);
  }
}

#define PADDINGVEC ((Vector2){padding, padding})

void Button::render() {
  DrawRectangleV(position - PADDINGVEC, textDems + PADDINGVEC + PADDINGVEC, WHITE);
  DrawRectangleV(position, textDems, BLACK);

  DrawTextEx(GetFontDefault(), text.c_str(), position + PADDINGVEC, 500, SPACING, WHITE);
}

#include "console.hpp"
#include "engine/entity.hpp"
#include "inputManager.hpp"
#include "raylib.h"
#include "player.hpp"
#include "raymath.h"

#include <iostream>

float Console::offset = 10;
float Console::borderWidth = 10;

Color Console::text = WHITE;
Color Console::window = BLACK;
Color Console::border = GRAY;

Console::Console() : Entity("Console") {
  open = false;

  oldCout = std::cout.rdbuf();
  std::cout.rdbuf(newCout.rdbuf());

  std::cout << "test" << '\n';
  std::cout << "test2" << '\n';

  InputManager* in = Player::player->getInputManager();
  in->addBind(keybind(KEY_GRAVE, [this](){
    open = !open;
    if(open)
      std::cout << "OPEN\n";
  }));
}

void Console::death() {
  std::cout.rdbuf(oldCout);
}

Console::~Console() {
}

void Console::postProcessingRender() {
  if(open) {
    const Vector2 textDems = MeasureTextEx(GetFontDefault(), "the quick brown fox jumped over the lazy dog 1234567890 !@#$%^&*()_+~[]:\"',./<>?", 30, 1);

    DrawRectangle(offset, offset, GetScreenWidth() - offset * 2, GetScreenHeight() - offset * 2, window);
    DrawText(newCout.str().c_str(), 2 * offset + borderWidth, 2 * offset + borderWidth, 30, text);

    DrawRectangle(offset, offset, borderWidth, GetScreenHeight() - offset * 2, border);
    DrawRectangle(offset, offset, GetScreenWidth() - offset * 2, borderWidth, border);

    DrawRectangle(GetScreenWidth() - offset - borderWidth, offset, borderWidth, GetScreenHeight() - offset * 2, border);
    DrawRectangle(offset, GetScreenHeight() - offset - borderWidth, GetScreenWidth() - offset * 2, borderWidth, border);
    DrawRectangle(offset, GetScreenHeight() - offset - borderWidth * 2 - textDems.y, GetScreenWidth() - offset * 2, borderWidth, border);
  }
}

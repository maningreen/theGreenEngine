#include "console.hpp"
#include "engine/entity.hpp"
#include "inputManager.hpp"
#include "mod.hpp"
#include "raylib.h"
#include "player.hpp"
#include "raymath.h"

#include <cctype>
#include <climits>
#include <cstdio>
#include <ios>
#include <iostream>
#include <sol/error.hpp>

float Console::offset = 10;
float Console::borderWidth = 10;

Color Console::text = WHITE;
Color Console::window = BLACK;
Color Console::border = GRAY;

Console::Console() : Entity("Console") {
  open = false;

  oldCout = std::cout.rdbuf();
  std::cout.rdbuf(newCout.rdbuf());
}

void Console::death() {
  std::cout.rdbuf(oldCout);
}

void Console::process(float delta) {
  if(!open) return;

  int x = GetKeyPressed();
  if(!x) return;

  bool shift = IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT);

  if(x == KEY_BACKSPACE) {
    if(typed.length() > 0)
      typed.pop_back();
  } else if(x == KEY_ENTER) {
    try {
      ModManager::getLua().script(typed);
      std::cout << typed << '\n';
    } catch (sol::error x) {
      std::cout << "ERROR: invalid expression\n";
    }
    typed.clear();
  } else {
    x = GetCharPressed();
    if(!x) return;
    if(shift) x = toupper(x);
    if(x == '`') {
      open = !open;
    } else
    typed.push_back(x);
  }
}

Console::~Console() {}

void Console::postProcessingRender() {
  if(open) {
    Vector2 textDems = 
      MeasureTextEx(GetFontDefault(), "the quick brown fox jumped over the lazy dog 1234567890 !@#$%^&*()_+~[]:\"',./<>?", 30, 1);
    const float spacing = 10;
    textDems.x += spacing * 2;
    textDems.y += spacing * 2;
    float totalTextHeight = MeasureTextEx(
      GetFontDefault(), 
      newCout.str().c_str(),
      30,
      2
    ).y;

    float height = GetScreenHeight();
    float width = GetScreenWidth();

    float totalTextSpace = height - offset * 2 - textDems.y - borderWidth;

    DrawRectangle(offset, offset, width - offset * 2, height - offset * 2, window);

    DrawTextPro(
      GetFontDefault(),
      newCout.str().c_str(),
      (Vector2){
        borderWidth + spacing + offset,
        borderWidth + spacing + offset
      },
      (Vector2){
        0,
        totalTextHeight - totalTextSpace
      },
      0,
      30,
      2,
      text
    );

    DrawRectangle(offset, offset, borderWidth, height - offset * 2, border);
    DrawRectangle(offset, offset, width - offset * 2, borderWidth, border);

    DrawRectangle(width - offset - borderWidth, offset, borderWidth, height - offset * 2, border);
    DrawRectangle(offset, height - offset - borderWidth, width - offset * 2, borderWidth, border);
    DrawRectangle(offset, height - offset * 2 - textDems.y - borderWidth, width - offset * 2, borderWidth, border);

    DrawTextPro(
      GetFontDefault(),
      typed.c_str(),
      (Vector2){
        offset + borderWidth + spacing,
        totalTextSpace + spacing * 2,
      },
      (Vector2){
        0,
        0,
      },
      0,
      30,
      2,
      text
    );
  }
}

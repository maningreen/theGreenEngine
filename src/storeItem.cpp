#include "storeItem.hpp"
#include "player.hpp"
#include "border.hpp"
#include <iostream>

#define vec4(a, b, c, d) \
  (Color){(unsigned char)((a) * 255), (unsigned char)((b) * 255), (unsigned char)((c) * 255), (unsigned char)((d) * 255)}

#define BACKCOLOUR vec4(0.0824, 0.0039, 0.0627, 1.0) // #150110

#define TEXTCOLOUR vec4(1.0000, 0.9490, 0.8706, 1.0)  // #FFF2DE

#define SEPERATORCOLOUR vec4(1.0000, 0.8157, 0.6745, 1.0) // #FFD0AC

float StoreItem::length = 200;
float StoreItem::padding = 100;

std::vector<std::function<void(StoreItem&)>> StoreItem::purchaseHooks;

#define BORDER_WIDTH 10

StoreItem::StoreItem(Mod& m, Vector2 p) :
  Entity2D(m.name, p), mod(m) {
  hovered = 0;
}

float theta = 0;

void StoreItem::process(float delta) {
  theta += delta;
  Vector2 mousePos = Border::wrapPos(Player::player->getCamera().getMousePosition());
  if(CheckCollisionPointRec(mousePos, (Rectangle){ position.x, position.y, length, length })) {
    if(hovered < 1)
      hovered += delta;
    else if(IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) // is released lets people drag away
      purchase();
  } else if(hovered > 0)
    hovered -= delta;
  if(hovered < 0)
    hovered = 0;
}


void StoreItem::render() {
  const float e = ease(hovered);
  const float l = .5f * length * e;
  DrawRectanglePro(
    (Rectangle){
      position.x + length / 2.0f,
      position.y + length / 2.0f,
      length + l,
      length + l
    },
    (Vector2){
      (length + l) / 2.0f,
      (length + l) / 2.0f
    },
    e * sin(theta) * 30,
    WHITE
  );
}

void StoreItem::postProcessingRender() {
  if(Player::player == nullptr) return;
  float e = ease(hovered);

  // Camera2D& cam = Player::player->getCamera().camera;

  std::string str = mod.name;
  str.append(1, '\n').append(5, '-').append(1, '\n').append(mod.description);

  Vector2 dems = MeasureTextEx(GetFontDefault(), str.c_str(), 300, 10);

  {
    // this helps remove any weird easings
    // if we just did the floor it would cause the final character to hang for a moment,
    // and if we did the ceil the first character would appear too early
    const int i = e < .5 
      ? std::floor(e * str.length())
      : std::ceil(e * str.length()); 
    str[i] = '\0';
  }

  Vector2 thingPos =
    // cam.target - cam.offset / cam.zoom +
    position + 
    (Vector2){-dems.x / 2, -300 - dems.y};
  DrawRectangleV(
    thingPos - (Vector2){padding + BORDER_WIDTH, padding + BORDER_WIDTH},
    (Vector2){
      (dems.x + 2 * (padding + BORDER_WIDTH)) * e,
      dems.y + 2 * (padding + BORDER_WIDTH)
    },
    SEPERATORCOLOUR
  );
  DrawRectangleV(
    thingPos - (Vector2){padding, padding},
    (Vector2){
      (dems.x + 2 * padding) * e,
      dems.y + 2 * padding
    },
    BACKCOLOUR
  );
  DrawTextEx(GetFontDefault(), str.c_str(), thingPos, 300, 10, TEXTCOLOUR);
}

float StoreItem::ease(float x) {
  // thanks easings.net for this
  return x < 0.5 ? 16 * x * x * x * x * x : 1 - std::pow(-2 * x + 2, 5) / 2;
}

void StoreItem::purchase() {
  for(std::function<void (StoreItem &)> f : purchaseHooks)
    f(*this);
  killDefered();
  Player::player->getModManager().addMod(mod, Player::player);
}

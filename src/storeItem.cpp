#include "storeItem.hpp"
#include "player.hpp"
#include "border.hpp"
#include "stdio.h"
#include "include.h"

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
  setState(Neutral);
  sigmaDelta = 0;
  sigmaDeltaPrime = 0;
  hovered = 0;
}

void StoreItem::process(float delta) {
  sigmaDelta += delta;
  Vector2 mousePos = Border::wrapPos(Player::player->getCamera().getMousePosition());
  bool isHovered =
    state != Passing && CheckCollisionPointRec(mousePos, (Rectangle){ position.x, position.y, length, length });
  switch (state) {
     case Passing:
      if(sigmaDelta >= 1)
        killDefered();
    break;
    case Hovered:
      if(!isHovered)
        setState(Neutral);
      else if(IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
        purchase();
      }
    break;
    case Neutral:
      if(isHovered)
        setState(Hovered);
    break;
  }
  if(isHovered)
    hovered = std::min(hovered + delta, 1.0f);
  else
    hovered = std::max(hovered - delta, 0.0f);
}

void StoreItem::render() {
  float e = ease(hovered);
  float l;
  {
    l = .5f * length * e;
    if(state == Passing)
      l += std::min(-(length + l) * ease(sigmaDelta), 0.0f);
  }
  DrawRectanglePro(
    (Rectangle){
      position.x + (length) / 2.0f,
      position.y + (length) / 2.0f,
      length + l,
      length + l
    },
    (Vector2){
      (length + l) / 2.0f,
      (length + l) / 2.0f
    },
    30 * sin(sigmaDeltaPrime + sigmaDelta) * hovered,
    WHITE
  );
}

void StoreItem::postProcessingRender() {
  float e = ease(std::min(hovered, 1.0f));

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
  Player::player->getModManager().addMod(mod, Player::player);
  for(std::function<void (StoreItem &)> f : purchaseHooks)
    f(*this);
  setState(Passing);
}

void StoreItem::setState(enum State s) {
  state = s;
  sigmaDeltaPrime += sigmaDelta;
  sigmaDelta = 0;
}

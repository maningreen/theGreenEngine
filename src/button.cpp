#include "button.hpp"
#include "player.hpp"
#include "border.hpp"
#include "stdio.h"
#include "include.h"

#define vec4(a, b, c, d) \
  (Color){(unsigned char)((a) * 255), (unsigned char)((b) * 255), (unsigned char)((c) * 255), (unsigned char)((d) * 255)}

#define BACKCOLOUR vec4(0.0824, 0.0039, 0.0627, 1.0) // #150110

#define TEXTCOLOUR vec4(1.0000, 0.9490, 0.8706, 1.0)  // #FFF2DE

#define SEPERATORCOLOUR vec4(1.0000, 0.8157, 0.6745, 1.0) // #FFD0AC

float Button::length = 200;
float Button::padding = 100;

Color Button::defaultColour = SEPERATORCOLOUR;
Color Button::hoveredColour = WHITE;

#define BORDER_WIDTH 10

Button::Button(Vector2 p, std::string t, std::function<void(Button*)> c) :
  Entity2D("Button", p),
  callback(c),
  text(t)
{
  textDems = MeasureTextEx(GetFontDefault(), t.c_str(), 300, 10);
  setState(Opening);
  sigmaDelta = 0;
  sigmaDeltaPrime = 0;
  hovered = 0;
}

Button::Button(Vector2 p, std::string t, bool pD, std::function<void(Button*)> c) :
  Entity2D("Button", p),
  callback(c),
  text(t)
{
  textDems = MeasureTextEx(GetFontDefault(), t.c_str(), 300, 10);
  postDip = pD;
  setState(Neutral);
  sigmaDelta = 0;
  sigmaDeltaPrime = 0;
  hovered = 0;
}

void Button::process(float delta) {
  sigmaDelta += delta;
  bool isHovered;
  if(state != Passing) {
    float l;
    {
      l = .5f * length * ease(hovered);
      if(state == Passing)
        l += std::min(-(length + l) * ease(sigmaDelta), 0.0f);
      else if(state == Opening)
        l += std::min(-(length + l) * ease(1 - sigmaDelta), 0.0f);
    }

    Vector2 mousePos = Border::wrapPos(Player::get().getCamera().getMousePosition());
    Vector2 relativeMousePos = mousePos - position - (Vector2){length / 2, length / 2};
    float theta = 30 * sin(sigmaDeltaPrime + sigmaDelta) * hovered * DEG2RAD;
    Vector2 angleVec = (Vector2){
      cos(theta),
      -sin(theta) // the - here is to *undo* the transformation
    };

    relativeMousePos = (Vector2){
      relativeMousePos.x * angleVec.x - relativeMousePos.y * angleVec.y, // [ cos(theta), 0 ]
      relativeMousePos.y * angleVec.x + relativeMousePos.x * angleVec.y  // [ 0, sin(theta) ] i think
    } + (Vector2){(length + l) / 2, (length + l) / 2}; // undo centering

    isHovered =
      relativeMousePos.x <= length + l &&
      relativeMousePos.y <= length + l &&
      relativeMousePos.x >= 0 &&
      relativeMousePos.y >= 0;
  } else isHovered = false;

  switch (state) {
    case Opening:
      if(sigmaDelta >= 1)
        setState(Neutral);
    break;
    case Passing:
      if(sigmaDelta >= 1) {
        if(postDip)
          callback((Button*)this);
        killDefered();
      }
    break;
    case Hovered:
      if(!isHovered)
        setState(Neutral);
      else if(IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
        if(!postDip)
          callback(this);
        setState(Passing);
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

void Button::render() {
  float e = ease(hovered);
  float l;
  {
    l = .5f * length * e;
    if(state == Passing)
      l += std::min(-(length + l) * ease(sigmaDelta), 0.0f);
    else if(state == Opening)
      l += std::min(-(length + l) * ease(1 - sigmaDelta), 0.0f);
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
    30 * sin(sigmaDeltaPrime + sigmaDelta) * e,
    ColorLerp(defaultColour, hoveredColour, e)
  );
}

void Button::postProcessingRender() {
  float e = ease(std::min(hovered, 1.0f));

  char scratch;
  // this helps remove any weird easings
  // if we just did the floor it would cause the final character to hang for a moment,
  // and if we did the ceil the first character would appear too early
  const int i = e < .5 
    ? std::floor(e * text.length())
    : std::ceil(e * text.length()); 
  scratch = text[i];
  text[i] = '\0';

  Vector2 thingPos =
    position + 
    (Vector2){-textDems.x / 2, -300 - textDems.y};
  DrawRectangleV(
    thingPos - (Vector2){padding + BORDER_WIDTH, padding + BORDER_WIDTH},
    (Vector2){
      (textDems.x + 2 * (padding + BORDER_WIDTH)) * e,
      textDems.y + 2 * (padding + BORDER_WIDTH)
    },
    SEPERATORCOLOUR
  );
  DrawRectangleV(
    thingPos - (Vector2){padding, padding},
    (Vector2){
      (textDems.x + 2 * padding) * e,
      textDems.y + 2 * padding
    },
    BACKCOLOUR
  );
  DrawTextEx(GetFontDefault(), text.c_str(), thingPos, 300, 10, TEXTCOLOUR);
  text[i] = scratch;
}

float Button::ease(float x) {
  // thanks easings.net for this
  return 
    x < 0.5
      ? 16 * x * x * x * x * x
      : 1 - std::pow(-2 * x + 2, 5) / 2;
}

void Button::setState(enum State s) {
  state = s;
  sigmaDeltaPrime += sigmaDelta;
  sigmaDelta = 0;
}

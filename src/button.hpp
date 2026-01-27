#ifndef BUTTON_HPP
#define BUTTON_HPP

#include "engine/entity.hpp"
#include <functional>

class Button : public Entity2D {
  std::function<void(Button*)> callback;

  Vector2 textDems;
  std::string text;

  bool postDip;

public:
  enum State {
    Hovered,
    Neutral,
    Passing,
    Opening
  };

  float sigmaDelta;
  float sigmaDeltaPrime;

  Button(Vector2, std::string, std::function<void(Button*)>);
  Button(Vector2, std::string, bool, std::function<void(Button*)>);

  void setState(enum State);

  void process(float delta) override;
  void render() override;
  void postProcessingRender() override;

  static float length;
  static float padding;

private:
  static float ease(float x);

  void purchase();

  float hovered;

  enum State state;

  static Color defaultColour;
  static Color hoveredColour;
};

#endif

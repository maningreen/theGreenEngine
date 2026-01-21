#ifndef BUTTON_HPP
#define BUTTON_HPP

#include "engine/entity.hpp"
#include <functional>

class Button : public Entity2D {
  std::function<void(Button&)> callback;

  Vector2 textDems;
  std::string text;

public:

  float padding;

  Button(
    Vector2 position,
    std::string text,
    std::function<void(Button&)> callback
  );

  const std::string& getText();
  void setText(std::string);

  void process(float delta) override;
  void render() override;

  static float defaultPadding;
};

#endif

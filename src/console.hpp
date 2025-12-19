#ifndef CONSOLE_H
#define CONSOLE_H

#include "engine/entity.hpp"
class Console : public Entity {

  std::streambuf *oldCout;
  std::ostringstream newCout;

  std::string typed;
  unsigned int index;

public:
  bool open;

  void postProcessingRender();

  void death() override;

  void process(float delta) override;

  static float offset;
  static float borderWidth;

  static Color text;
  static Color window;
  static Color border;

  Console();
  ~Console();
};

#endif

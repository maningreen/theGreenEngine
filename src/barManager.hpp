#ifndef barMan_h
#define barMan_h

#include "bars.hpp"
#include "engine/entity.hpp"

class BarManager : public Entity {
private:
  Bar bar;
  Vector2* positionPointer;

public:
  Bar* getBar();
  Vector2* getPositionPointer();
  float targetDistance;

  BarManager(Vector2 Position, float targetDistance, Bar childBar);
  BarManager(Vector2 *Position, float targetDistance, Bar childBar);
  ~BarManager();

  void setBarPercentage(float p);

  void process(float delta) override;
  void render() override;
};

#endif

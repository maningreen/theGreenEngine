#ifndef barMan_h
#define barMan_h

#include "bars.hpp"
#include "entity.hpp"

class BarManager : public Entity2D {
private:
  Bar *bar;
  Vector2 *positionPointer;

public:
  Bar *getBar();
  float targetDistance;

  BarManager(Vector2 Position, float targetDistance, Bar* childBar);
  BarManager(Vector2 *Position, float targetDistance, Bar* childBar);
  ~BarManager();

  void setBarPercentage(float p);

  void Process(float delta) override;
};

#endif

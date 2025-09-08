#ifndef laser_h
#define laser_h

#include "engine/entity.hpp"

class Laser : public Entity2D {
private:
  static Color defaultColour;
  static float defaultLength;
  static float defaultWidth;

public:
  float rotation;
  float length;
  float width;
  bool shouldRender;
  Color colour;

  void Render() override;

  bool getBreaks();
  bool getBreaksX();
  bool getBreaksY();

  Vector2 getBreakingPoint();
  Vector2 getEndPoint();

  void lookAt(Vector2 v);

  static Color getDefaultColour();
  static float getDefaultLength();
  static float getDefaultWidth();

  Laser(Vector2 p, float rot, float len, Color col);
  Laser(Vector2 p, float rot, float len);
  Laser(Vector2 p, float rot);
  Laser(Vector2 p);
};

#endif

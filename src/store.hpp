#ifndef store_h
#define store_h money

#include "engine/entity.hpp"
#include <string>

template <typename t>
class UpgradeManager {
public:
  //upCount will be rounded if it's an integer
  static void upgrade(t* ptr, float upCount);
};

template <typename t>
class StoreItem : public Entity2D {

private:
  static Vector2 stdDimensions;
  static float borderWidth;

public:
  StoreItem(std::string name, t* ptr);
  t* ptr;
  ~StoreItem();

  void Process(float delta) override;
  void Render() override;

  static Vector2 getStdDimensions();
  static float getBorderWidth();
};

#endif

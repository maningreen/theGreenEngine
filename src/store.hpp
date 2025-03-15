#ifndef store_h
#define store_h money

#include "engine/entity.hpp"
#include <string>

template <typename t>
class StoreItem : public Entity2D {

private:
  static Vector2 stdDimensions;
  static float borderWidth;

  float purchaseProgress;

public:
  static float upgradePercent;
  static float purchaseTime;

  StoreItem(std::string name, t* ptr);
  t* ptr;
  ~StoreItem();

  void Process(float delta) override;
  void Render() override;

  void upgrade(float perc);

  float getPurchaseProgress();

  static Vector2 getStdDimensions();
  static float getBorderWidth();
};

#endif

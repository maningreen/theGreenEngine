#ifndef store_h
#define store_h money

#include "engine/entity.hpp"

class UpgradeManager {
public:
  //upCount will be rounded if it's an integer
  template <typename t>
  static void upgrade(t* ptr, float upCount);
};

class StoreItem : public Entity2D {
public:
  StoreItem();
  ~StoreItem();
};

#endif

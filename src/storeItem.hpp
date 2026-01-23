#ifndef STOREITEM_H
#define STOREITEM_H

#include "engine/entity.hpp"
#include "mod.hpp"
#include "button.hpp"

class StoreItem : public Button {
public:
  Mod mod;

  StoreItem(Mod m, Vector2 p);

  static std::vector<std::function<void(StoreItem&)>> purchaseHooks;
private:
  void purchase();
};

#endif

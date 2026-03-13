#ifndef STOREITEM_H
#define STOREITEM_H

#include "button.hpp"
#include "engine/entity.hpp"
#include "mod.hpp"

class StoreItem : public Button {
  public:
    Mod mod;

    StoreItem(Mod m, Vector2 p);

    static std::vector<std::function<bool(StoreItem&)>> purchaseHooks;

  private:
    void purchase();
};

#endif

#ifndef STOREITEM_H
#define STOREITEM_H

#include "button.hpp"
#include "engine/entity.hpp"
#include "mod.hpp"

class StoreItem : public Button {
  public:
    Mod mod;

    StoreItem(Mod m, Vector2 p);

    static const std::string purchaseEvent;

  private:
    void purchase();
};

#endif

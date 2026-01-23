#include "storeItem.hpp"
#include "player.hpp"
#include "border.hpp"
#include "stdio.h"
#include "include.h"

std::vector<std::function<bool(StoreItem&)>> StoreItem::purchaseHooks;

StoreItem::StoreItem(Mod& m, Vector2 p) :
  Button(p, m.name + "\n-----\n" + m.description, [this](Button&){ purchase(); }), mod(m) {
}

void StoreItem::purchase() {
  Player::get().getModManager().addMod(mod, &Player::get());
  for(
    std::function<bool(StoreItem&)>* f = purchaseHooks.data();
    f <purchaseHooks.size() + purchaseHooks.data();
  ) if((*(f++))(*this))
      purchaseHooks.erase(purchaseHooks.begin() + (--f - purchaseHooks.data()));
  setState(Passing);
}

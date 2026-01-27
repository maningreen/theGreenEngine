#include "storeItem.hpp"
#include "player.hpp"
#include "border.hpp"
#include "stdio.h"
#include "include.h"

std::vector<std::function<void(StoreItem&)>> StoreItem::purchaseHooks;

StoreItem::StoreItem(Mod m, Vector2 p) :
  Button(p, m.name + "\n-----\n" + m.description, [](Button* self){ ((StoreItem*)self)->purchase(); }), mod(m) {
}

void StoreItem::purchase() {
  Player::getPtr()->getModManager()->addMod(mod, &Player::get());
  for(std::function<void(StoreItem&)>& t : purchaseHooks)
    t(*this);
  purchaseHooks.erase(purchaseHooks.begin(), purchaseHooks.end());
  setState(Passing);
}

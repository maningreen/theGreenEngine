#include "storeItem.hpp"

#include "border.hpp"
#include "include.h"
#include "player.hpp"
#include "stdio.h"

std::vector<std::function<bool(StoreItem&)>> StoreItem::purchaseHooks;

StoreItem::StoreItem(Mod m, Vector2 p)
  : Button(
      p,
      m.name + "\n-----\n" + m.description,
      [](Button* self) {
          ((StoreItem*)self)->purchase();
      }
    ),
    mod(m) {}

void StoreItem::purchase() {
    Player::getPtr()->getModManager()->addMod(mod, Player::getPtr());
    for(std::function<bool(StoreItem&)>& t : purchaseHooks) t(*this);
    for(int i = 0; i < purchaseHooks.size(); i++) {
        bool x = purchaseHooks[i](*this);
        if(x) {
            purchaseHooks[i] = purchaseHooks.back();
            purchaseHooks.pop_back();
            i -= 1;
        }
    }
    setState(Passing);
}

#include "storeItem.hpp"

#include "border.hpp"
#include "engine/world.hpp"
#include "include.h"
#include "player.hpp"
#include "stdio.h"

const std::string StoreItem::purchaseEvent = "purchase";

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
    std::cout << "TEST\n";
    ModManager::get()->addMod(mod, Player::getPtr());
    World::callEvent(purchaseEvent, &mod);
    setState(Passing);
}

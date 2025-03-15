#include "store.hpp"
#include "player.hpp"

template <typename t>
void UpgradeManager::upgrade(t *ptr, float upCount) {
  *ptr += upCount;
}

StoreItem::StoreItem() : Entity2D("StoreItem", Vector2Zero()){
  UpgradeManager::upgrade<float>(&Player::dashControl, 60);
}

StoreItem::~StoreItem() {}

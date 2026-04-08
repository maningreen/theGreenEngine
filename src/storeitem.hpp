#ifndef STOREITEM_H
#define STOREITEM_H

#include "button.hpp"
#include "engine/entity.hpp"
#include "mod.hpp"

namespace StoreItem {
Button init(Mod mod);

static const std::string purchaseEvent = "storeItemPurchased";

static const unsigned maxParticleCount = 75;
static const unsigned minParticleCount = 50;

static const float maxParticleSpeed = 1000;
static const float minParticleSpeed = 500;
};  // namespace StoreItem

#endif

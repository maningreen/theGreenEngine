#ifndef STOREITEM_H
#define STOREITEM_H

#include "button.hpp"
#include "engine/entity.hpp"
#include "mod.hpp"

namespace StoreItem {
Button init(Mod mod);

static const unsigned maxParticleCount = 10;
static const unsigned minParticleCount = 5;

static const float maxParticleSpeed = 300;
static const float minParticleSpeed = 100;
};  // namespace StoreItem

#endif

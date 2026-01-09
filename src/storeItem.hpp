#ifndef STOREITEM_H
#define STOREITEM_H

#include "engine/entity.hpp"
#include "mod.hpp"

class StoreItem : public Entity2D {
private:
  static float ease(float x);
public:
  Mod mod;

  float hovered;

  StoreItem(Mod& m, Vector2 p);

  void process(float delta) override;
  void render() override;
  void postProcessingRender() override;

  static float length;
  static float padding;
};

#endif

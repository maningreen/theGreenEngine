#ifndef STOREITEM_H
#define STOREITEM_H

#include "engine/entity.hpp"
#include "mod.hpp"


class StoreItem : public Entity2D {
public:
  enum State {
    Hovered,
    Neutral,
    Passing,
    Opening
  };
  Mod mod;

  float sigmaDelta;
  float sigmaDeltaPrime;

  StoreItem(Mod& m, Vector2 p);

  void setState(enum State);

  void process(float delta) override;
  void render() override;
  void postProcessingRender() override;

  static float length;
  static float padding;

  static std::vector<std::function<bool(StoreItem&)>> purchaseHooks;

private:
  static float ease(float x);

  void purchase();

  float hovered;

  enum State state;

  static Color defaultColour;
  static Color hoveredColour;
};

#endif

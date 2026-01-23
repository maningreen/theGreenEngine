#include "store.hpp"
#include "engine/core.h"
#include "storeItem.hpp"

std::string Store::tag = "StoreItem";

Store::Store() : Entity("StoreManager") {
  closing = false;
  // load mods from the pool
  std::list<std::string> x = ModManager::listPoolMods();
  std::vector<std::string> arr(x.begin(), x.end());
  for(int i = 0; i < 3 && arr.size() > 0; i++) {
    int n = rand() % arr.size();
    std::optional<Mod> mod = Mod::fromName(arr[n]);
    if(!mod.has_value()) { 
      arr.erase(arr.begin() + n);
      return;
    }
    StoreItem x = StoreItem(
      mod.value(),
      (Vector2){
        2 * Button::length * (float)i - Button::length * 2.5f,
        -Button::length / 2.5f
      }
    );
    items.push_back(x);
    arr.erase(arr.begin() + n);
  }

  StoreItem::purchaseHooks.push_back(
    [this](StoreItem& x){
      this->close();
      for(StoreItem& item : items)
        item.setState(StoreItem::Passing);
      return true;
    }
  );
}

void Store::process(float delta) {
  for(int i = 0; i < items.size(); i++) {
    items[i].process(delta);
    if(!items[i].getValid())
      items.erase(items.begin() + i--);
  }
  sigmaDelta += delta;
  if(closing) {
    if(sigmaDelta >= 1)
      killDefered();
  }
}

void Store::render() {
  float l = (StoreItem::length * 3.0f + 30) * std::min(ease(closing ? 1 - sigmaDelta : sigmaDelta), 1.0f);
  float h = (StoreItem::length * .75f + 30);
  DrawRectangle(-l + 15, -h + 15, l * 2, h * 2, YELLOW);
  l -= 30;
  h -= 30;
  DrawRectangle(-l + 15, -h + 15, l * 2, h * 2, BLACK);

  for(StoreItem& x : items)
    x.render();
}

void Store::postProcessingRender() {
  for(StoreItem& x : items)
    x.postProcessingRender();
}

float Store::ease(float x) {
  // thanks easings.net for this
  return x < 0.5 ? 16 * x * x * x * x * x : 1 - std::pow(-2 * x + 2, 5) / 2;
}

void Store::close() {
  sigmaDelta = 0;
  closing = true;
}

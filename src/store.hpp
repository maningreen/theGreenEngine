#ifndef STORE_H
#define STORE_H

#include "mod.hpp"
#include "storeItem.hpp"

// manages the *during* store, not outside of store
class Store : public Entity {
private:
  static std::string tag;
  std::vector<StoreItem> items;
public:
  Store();
  void process(float delta) override;
  void render() override;
  void postProcessingRender() override;
};

#endif

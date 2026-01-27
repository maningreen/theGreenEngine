#ifndef STORE_H
#define STORE_H

#include "mod.hpp"
#include "storeItem.hpp"

// manages the *during* store, not outside of store
class Store : public Entity {
private:
  static std::string tag;
  std::vector<StoreItem> items;
  bool closing;
  float sigmaDelta;

  static float ease(float x);
  void close();
public:
  Store();
  void process(float delta) override;
  void render() override;
  void death() override;
  void postProcessingRender() override;

  std::vector<std::function<void(void)>> storeCloseCallbacks;
};

#endif

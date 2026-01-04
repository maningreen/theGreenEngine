#ifndef STORE_H
#define STORE_H

#include "mod.hpp"

// manages the *during* store, not outside of store
class Store : public Entity {
private:
  std::vector<Mod> mods;
public:
  Store();
  void process(float delta) override;
  void postProcessingRender() override;
};

#endif

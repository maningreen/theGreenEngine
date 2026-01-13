#ifndef STORE_H
#define STORE_H

#include "mod.hpp"

// manages the *during* store, not outside of store
class Store : public Entity {
private:
  static std::string tag;
public:
  Store();
  void process(float delta) override;
};

#endif

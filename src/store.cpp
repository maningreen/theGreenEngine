#include "store.hpp"

Store::Store() : Entity("StoreManager") {
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
    mods.push_back(mod.value());
    arr.erase(arr.begin() + n);
  }
}

void Store::process(float delta) {}

void Store::postProcessingRender() {
  for(int i = 0; i < mods.size(); i++) {
    DrawText(mods[i].description.c_str(), 0, 100 * i, 300, WHITE);
  }
}

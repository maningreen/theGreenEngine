#include "store.hpp"
#include "storeItem.hpp"

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
    addChild(
      new StoreItem(
        mod.value(),
        (Vector2){
          2 * StoreItem::length * (float)i - StoreItem::length * 2.5f,
          -StoreItem::length / 2.5f
        }
      )
    ); 
    arr.erase(arr.begin() + n);
  }

  StoreItem::purchaseHooks.push_back(
    [this](StoreItem& x){
      this->killDefered();
    }
  );
}

void Store::process(float delta) {}

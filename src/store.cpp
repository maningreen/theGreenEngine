#include "store.h"

template <typename t>
void UpgradeManager::upgrade(t, void *ptr, float upCount) {
  t* typedPointer = (t*)ptr;
  typedPointer += upCount;
}

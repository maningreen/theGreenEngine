#include "store.hpp"

#include "engine/world.hpp"
#include "storeItem.hpp"

const std::string Store::storeCloseEvent = "storeClose";

float clampf(float low, float high, float value) {
    return value;
}

extern "C" {
void drawStoreBody(float length, float sigmaDelta, float (*const ease)(float));
}

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
        items.push_back(StoreItem(
          mod.value(),
          (Vector2){2 * Button::length * (float)i - Button::length * 2.5f, -Button::length / 2.5f}
        ));
        arr.erase(arr.begin() + n);
    }

    World::listenEvent(StoreItem::purchaseEvent, &Store::purchaseCallback, getId());
}

void Store::process(float delta) {
    for(int i = 0; i < items.size(); i++) {
        items[i].process(delta);
        if(!items[i].getValid()) items.erase(items.begin() + i--);
    }
    sigmaDelta += delta;
    if(closing)
        if(sigmaDelta >= 1) killDefered();
}

void Store::render() {
    drawStoreBody(StoreItem::length, sigmaDelta, &ease);

    for(StoreItem& x : items) x.render();
}

void Store::postProcessingRender() {
    for(StoreItem& x : items) x.postProcessingRender();
}

extern "C" {
float Store::ease(float x) {
    // thanks easings.net for this
    return x < 0.5 ? 16 * x * x * x * x * x : 1 - pow(-2 * x + 2, 5) / 2;
}
}

void Store::close() {
    sigmaDelta = 0;
    closing = true;
    World::callEvent(storeCloseEvent, nullptr);
}

void Store::death() {}

void Store::purchaseCallback(Entity* t, void*) {
    Store* self = (Store*)t;
    for(Button& item : self->items) item.setState(StoreItem::Passing);
    self->close();
}

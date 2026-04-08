#include "store.hpp"

#include "button.hpp"
#include "engine/world.hpp"
#include "player.hpp"
#include "storeitem.hpp"

extern "C" void drawStoreBody(
  float length,
  float sigmaDelta,
  float (*const ease)(float)  // the easing function we use
);

Store::Store() : Entity("Store") {
    state = opening;
    stateTime = 0;
}

void Store::init() {
    // spawn the tres store items
    for(int i = -1; i < 2; i++) {
        // get out target X value, which is
        const float targetX = 2.0f * Button::length * i;
        const std::string m = Player::get().getModManager()->pickRandomPoolMod();
        const Mod mod = Player::get().getModManager()->fromName(m).value();
        Button* const button = new Button(StoreItem::init(mod));
        button->position.x = targetX;
        World::listenEvent(StoreItem::purchaseEvent, &purchaseCallback, button->getId());
        World::addEntity(button);
    }
}

void Store::process(float delta) {
    stateTime += delta;
}

void Store::render() {
    drawStoreBody(Button::length, stateTime, &Store::ease);
}

float Store::ease(float x) {
    return x < 0.5 ? 16 * x * x * x * x * x : 1 - pow(-2 * x + 2, 5) / 2;
}

/// This is *strictly* for the buttons
void Store::purchaseCallback(Entity* t, Button* called, const Mod* mod) {
    Button* const self = (Button*)t;
    if(self->getState() != Button::Passing)
        self->setState(Button::Passing);
    if(self == called)
        Player::addMod(*mod);
}

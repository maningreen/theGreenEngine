#include "storeitem.hpp"

#include "engine/world.hpp"
#include "particle.hpp"
#include "player.hpp"

void purchaseCallback(Entity* en, Button* caller, Mod& m) {
    Button* self = (Button*)en;
    self->postDip = false;
    self->callback = [](Button* self) {
    };
    self->setState(Button::Passing);
}

void purchase(Button* button, Mod m) {
    const unsigned particleCount = (((float)rand() / (float)RAND_MAX) *
                                    (StoreItem::maxParticleCount - StoreItem::minParticleCount)) +
                                   StoreItem::minParticleCount;

    for(int i = 0; i < particleCount; i++) {
        const float theta = M_PI * 2 * (float)rand() / (float)RAND_MAX;
        const float speed = ((float)rand() / (float)RAND_MAX) *
                              (StoreItem::maxParticleSpeed - StoreItem::minParticleSpeed) +
                            StoreItem::maxParticleSpeed;
        const Vector2 vel = (Vector2){cosf(theta) * speed, sinf(theta) * speed};
        World::addEntity(new Particle(button->position, vel, 20, false));
    }

    World::callEvent<Button*, const Mod*>(
      StoreItem::purchaseEvent,
      std::tuple<Button*, const Mod*>(button, &m)
    );
}

Button StoreItem::init(Mod mod) {
    std::function<void(Button*)> callback = [mod](Button* self) {
        purchase(self, mod);
    };
    std::string str = mod.name + "\n-----\n" + mod.description;
    Button b = Button(Vector2Zero(), str, true, callback);
    World::listenEvent(StoreItem::purchaseEvent, &purchaseCallback, b.getId());
    b.addTag(Tags::storeItem);
    return b;
}

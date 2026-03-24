#include "storeitem.hpp"

#include "engine/world.hpp"
#include "particle.hpp"
#include "player.hpp"

Button StoreItem::init(Mod mod) {
    std::function<void(Button *)> callback = [mod](Button *self) {
        const unsigned particleCount =
          ((float)rand() / (float)RAND_MAX * (maxParticleCount - minParticleCount)) +
          minParticleCount;
        for(int i = 0; i < particleCount; i++) {
            const float theta = M_PI * 2 * (float)rand() / (float)RAND_MAX;
            const float speed =
              ((float)rand() / (float)RAND_MAX) * (maxParticleSpeed - minParticleCount) +
              maxParticleSpeed;
            const Vector2 vel = (Vector2){cosf(theta) * speed, sinf(theta) * speed};
            World::addEntity(new Particle(self->position, vel));
        }
        Player::get().getModManager()->addMod(mod, Player::get().getId());
    };
    std::string str = mod.name + "\n-----\n" + mod.description;
    return Button(Vector2Zero(), str, false, callback);
}

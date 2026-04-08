//! TODO: add support for dynamic store item count

#ifndef STORE_H
#define STORE_H

#include "engine/entity.hpp"
#include "button.hpp"
#include "mod.hpp"

struct Store : public Entity {
    enum State {
        opening,
        neutral,
        closing,
    };
    void init() override;
    void render() override;
    void process(float delta) override;

    State state;
    /// the time in the state.
    float stateTime;

    ~Store() = default;
    Store();

    static float ease(float x);

  private:
    static void purchaseCallback(Entity*, Button* self, const Mod*);
};

#endif

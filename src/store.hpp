//! TODO: add support for dynamic store item count

#ifndef STORE_H
#define STORE_H

#include "button.hpp"
#include "engine/entity.hpp"
#include "mod.hpp"

struct Store : public Entity {
    enum State {
        opening,
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

    static const std::string closeEvent;

  private:
    static void purchaseCallback(Entity*, Button* self, const Mod*);
};

#endif

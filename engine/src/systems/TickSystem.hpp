/**
 * ActorTickSystem calls tick on all actors every loop
 */

#pragma once
#include "System.hpp"
#include "core/Engine.hpp"

namespace R3 {

template <bool Tick>
class ECSBase;

template <typename T>
concept Tickable = requires(T t) { t.tick(double{}); };

template <Tickable T>
struct TickSystem : public System {
    void tick(double dt) override {
        Engine::activeScene().componentView<T>().each([=](T& ent) { ent.tick(dt); });
    }
};

} // namespace R3
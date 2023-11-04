#pragma once
#include "System.hpp"
#include "api/Log.hpp"
#include "core/Engine.hpp"

namespace R3 {

template <typename T>
struct EntitySubSystem : public System {
    void tick(double dt) override {
        Engine::activeScene().componentView<T>().each([=](T& ent) { ent.tick(dt); });
    }
};

} // namespace R3
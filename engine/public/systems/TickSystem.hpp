#pragma once
#include "System.hpp"
#include "core/Engine.hpp"

namespace R3 {

/// @brief checks for `void tick(double)` method present
template <typename T>
concept Tickable = requires(T t) { t.tick(double{}); };

/// @brief Tick system is used to class tick on a `Tickable` object once per frame
/// Entity will register this TickSytem with an Derived Entity T if that entity implements the Tickable concept
/// @tparam T a Tickable Entity
template <Tickable T>
struct TickSystem : public System {
    /// @brief Standard R3 tick method
    /// @param dt delta time in milliseconds
    void tick(double dt) override {
        Engine::activeScene()->componentView<T>().each([=](T& ent) { ent.tick(dt); });
    }
};

} // namespace R3
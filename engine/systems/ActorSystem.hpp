/// @file ActorSystem.hpp

#pragma once

#include <concepts>
#include <entt/resource/resource.hpp>
#include "System.hpp"
#include "engine/api/Api.hpp"
#include "engine/core/Engine.hpp"

namespace R3 {

/// @brief ActorSystem is a template system that updates all components of type T that satisfy the Updatable concept.
///
/// This allows for easy creation of new systems by simply defining a component with an update method and registering it
/// with the world, this is used for simple actor-like behavior where the logic is contained within the component
/// itself, this is not suitable for more complex systems that require multiple components or more control over the
/// update order, in those cases a custom system should be created by inheriting from ISystem.
/// @tparam T component type to update, must satisfy the Updatable concept
template <Updatable T>
class R3_API ActorSystem : public ISystem {
public:
    virtual ~ActorSystem() noexcept override {}

    virtual void update(double dt) override {
        GEngine()->World().registry().view<T>().each([=](T& component) { component.update(dt); });
    }
};

} // namespace R3
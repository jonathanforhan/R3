#pragma once
#include <entt/entt.hpp>
#include "api/Types.hpp"

namespace R3 {

class Scene {
public:
    Scene() = default;

    template <typename... T>
    [[nodiscard]] decltype(auto) componentView();

    template <typename... T>
    [[nodiscard]] decltype(auto) componentView() const;

public:
    mat4 view{1.0f};
    mat4 projection{1.0f};

private:
    entt::registry m_registry;

    // Entity is a friend class to access the registry
    // a lot of entity management is done through Entity
    // this provides cleaner api at cost of being non-local
    friend class Entity;
    friend class Engine;
};

template <typename... T>
inline decltype(auto) Scene::componentView() {
    return m_registry.view<T...>();
}

template <typename... T>
inline decltype(auto) Scene::componentView() const {
    return m_registry.view<T...>();
}

} // namespace R3
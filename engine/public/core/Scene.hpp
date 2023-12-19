#pragma once

#include <entt/entt.hpp>
#include "api/Types.hpp"
#include "systems/System.hpp"

namespace R3 {

class Scene {
public:
    Scene() = default;

    template <typename... T>
    [[nodiscard]] decltype(auto) componentView();

    template <typename... T>
    [[nodiscard]] decltype(auto) componentView() const;

    template <typename T, typename... Args>
    requires requires { std::is_base_of_v<System, T>; }
    void addSystem(Args&&... args);

private:
    void runSystems(double dt);

private:
    entt::registry m_registry;
    std::vector<std::unique_ptr<System>> m_systems;
    std::set<std::string> m_systemSet;

    // Entity is a friend class to access the registry
    // a lot of entity management is done through Entity
    // this provides cleaner api at cost of being non-local
    friend class Entity;
    friend class Engine;
};

} // namespace R3

#include "core/Scene.ipp"
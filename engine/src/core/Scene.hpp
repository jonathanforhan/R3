#pragma once
#include <entt/entt.hpp>
#include "api/Log.hpp"
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
    void addSystem(Args&&... args);

private:
    void runSystems(double dt);

public:
    mat4 view{1.0f};
    mat4 projection{1.0f};

private:
    entt::registry m_registry;
    std::vector<std::unique_ptr<System>> m_systems;
    std::set<std::string> m_system_set;

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

template <typename T, typename... Args>
inline void Scene::addSystem(Args&&... args) {
    static_assert(std::is_base_of_v<System, T>);
    if (!m_system_set.contains(typeid(T).name())) {
        m_systems.emplace_back(new T(std::forward<Args>(args)...));
        m_system_set.emplace(typeid(T).name());
    }
}

inline void Scene::runSystems(double dt) {
    // tick our systems
    for (const auto& system : m_systems) {
        system->tick(dt);
    }
}

} // namespace R3
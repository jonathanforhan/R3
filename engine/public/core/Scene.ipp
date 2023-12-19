#include "core/Scene.hpp"

namespace R3 {

template <typename... T>
inline decltype(auto) Scene::componentView() {
    return m_registry.view<T...>();
}

template <typename... T>
inline decltype(auto) Scene::componentView() const {
    return m_registry.view<T...>();
}

template <typename T, typename... Args>
requires requires { std::is_base_of_v<System, T>; }
inline void Scene::addSystem(Args&&... args) {
    if (!m_systemSet.contains(typeid(T).name())) {
        m_systems.emplace_back(new T(std::forward<Args>(args)...));
        m_systemSet.emplace(typeid(T).name());
    }
}

inline void Scene::runSystems(double dt) {
    // tick our systems
    for (const auto& system : m_systems) {
        system->tick(dt);
    }
}

} // namespace R3
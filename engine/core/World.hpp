#pragma once

#include <memory>
#include <set>
#include <typeindex>
#include <vector>
#include <entt/entt.hpp>
#include "api/Assert.hpp"
#include "api/Class.hpp"
#include "core/Camera.hpp"
#include "systems/System.hpp"

namespace R3 {

class World {
private:
    R3_CTOR_DEFAULT(World);

public:
    static World& instance();

    template <typename T, typename... Args>
    void addSystem(Args&&... args) {
        R3_ASSERT(!m_systemSet.contains(typeid(T)) && "System already registered");
        m_systems.emplace_back(new T(std::forward<Args>(args)...));
        m_systemSet.insert(typeid(T));
    }

    entt::registry& registry() noexcept { return m_registry; }
    const entt::registry& registry() const noexcept { return m_registry; }

    Camera& camera() noexcept { return m_camera; }
    const Camera& camera() const noexcept { return m_camera; }

private:
    /// @brief Called once per frame by the Engine to update all systems
    /// @param dt
    void update(double dt);

private:
    entt::registry m_registry;
    std::vector<std::unique_ptr<ISystem>> m_systems;
    std::set<std::type_index> m_systemSet;
    Camera m_camera;

private:
    friend class Engine;
};

} // namespace R3
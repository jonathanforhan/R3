#pragma once

#include <memory>
#include <set>
#include <typeindex>
#include <typeinfo>
#include <vector>
#include <entt/entt.hpp>
#include "engine/api/Api.hpp"
#include "engine/api/Class.hpp"
#include "engine/core/Camera.hpp"
#include "engine/core/Log.hpp"
#include "engine/systems/System.hpp"

namespace R3 {

class R3_API World {
private:
    R3_CTOR_DEFAULT(World);

public:
    template <typename T, typename... Args>
    void addSystem(Args&&... args) {
        if (!m_systemSet.contains(typeid(T))) {
            m_systems.emplace_back(new T(std::forward<Args>(args)...));
            m_systemSet.insert(typeid(T));
        } else {
            LOG_WARNING("System: {} already added", typeid(T).name());
        }
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
    friend class R3_API Engine;
};

} // namespace R3
#pragma once

#include <memory>
#include <set>
#include <string>
#include <typeindex>
#include <typeinfo>
#include <unordered_map>
#include <vector>
#include <entt/entt.hpp>
#include "engine/api/Api.hpp"
#include "engine/api/Class.hpp"
#include "engine/core/Camera.hpp"
#include "engine/core/Engine.hpp"
#include "engine/core/Entity.hpp"
#include "engine/core/Log.hpp"
#include "engine/systems/ActorSystem.hpp"
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

    void clear() noexcept;

    entt::registry& registry() noexcept { return m_registry; }
    const entt::registry& registry() const noexcept { return m_registry; }

    Camera& camera() noexcept { return m_camera; }
    const Camera& camera() const noexcept { return m_camera; }

    /// @brief Registers a component factory function tied to a string name for deserialization, the function takes an
    /// entity and adds the component to it
    /// @tparam T component type to register, should be default constructible as the lambda will call emplace<T> with no
    /// @param name string name of the component type, should be unique and consistent with the name used in
    /// serialization
    /// @param fn function pointer that takes an entity and adds the component to it, this is used for deserialization
    /// to add
    template <typename T>
    void registerComponentFactory(const std::string& name, void (*fn)(Entity)) {
        m_componentFactoryMap[name] = fn;

        if constexpr (Updatable<T>) {
            addSystem<ActorSystem<T>>();
        }
    }

    /// @brief Helper function to register a component factory using a lambda that constructs the component in place,
    /// this is more
    /// @tparam T component type to register, should be default constructible as the lambda will call emplace<T> with no
    /// @param name string name of the component type, should be unique and consistent with the name used in
    /// serialization
    template <typename T>
    void registerComponentFactory(const std::string& name) {
        registerComponentFactory(name, [](Entity entity) { m_registry.emplace<T>(entity); });
    }

    /// @brief Unregisters a component factory function by name, this can be used to remove component types that are no
    /// longer
    /// @param name string name of the component type to unregister
    void unregisterComponentFactory(const std::string& name) {
        if (m_componentFactoryMap.contains(name)) {
            m_componentFactoryMap.erase(name);
        }
    }

    bool addRegisteredComponentByName(const std::string& name, Entity entity) {
        if (m_componentFactoryMap.contains(name)) {
            m_componentFactoryMap[name](entity);
            return true;
        }
        LOG_WARNING("Component factory for {} not found", name);
        return false;
    }

private:
    /// @brief Called once per frame by the Engine to update all systems
    /// @param dt
    void update(double dt);

private:
    entt::registry m_registry;

    std::vector<std::unique_ptr<ISystem>> m_systems;
    std::set<std::type_index> m_systemSet;

    // register 'component factories' or functions that create components tied to a string name for deserialization, the
    // function takes an entity and adds the component to it
    std::unordered_map<std::string, void (*)(Entity entity)> m_componentFactoryMap;

    Camera m_camera;

private:
    template <Updatable T>
    friend class ActorSystem;
    friend class R3_API Engine;
};

} // namespace R3
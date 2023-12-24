#pragma once

/// @file Entity.hpp
/// @brief Provides the Entity part of the Entity Component System

#include <entt/entt.hpp>
#include "api/Construct.hpp"
#include "components/TransformComponent.hpp"
#include "core/Scene.hpp"
#include "systems/TickSystem.hpp"

namespace R3 {

/// @brief Used to check if we can/must initialize an Entity on Entity::create<Derived>
template <typename T>
concept Initializable = requires(T t) { t.init(); };

/// @brief Entity base class that other Entity subclasses can derive
///
/// when an entity is created with Entity::create<Derived> *if* it is Tickable
/// (implements tick(double) method [checked through a C++20 concept]) then
/// we add a TickSystem for the entity and it's tick function gets called every frame
/// there is no virtual function overhead with the tick system just a normal c++ method call
/// if the class is `Initializable` (implements init() method) that will be called as well
class Entity {
public:
    DEFAULT_CONSTRUCT(Entity);
    NO_COPY(Entity);
    DEFAULT_MOVE(Entity);

    /// @brief Get the Entity's owning scene
    /// @return parent scene
    Scene* parentScene() { return m_parentScene; }

    /// @brief Query whether or not the Entity is valid in the Registry.
    /// @return validity
    [[nodiscard]] bool valid() const;

    /// @brief Create an Entity class/subclass bound to a controling scene.
    /// the entity created must derivate from Entity, only for the id and parentScene fields.
    /// We store it as it's type, not an Entity pointer so Entity does not need a virtual descructor
    /// @tparam T Entity type
    /// @tparam ...Args constructor args
    /// @param ...args forwarded to the Entity-Derived constructor
    /// @return reference to created entity
    template <typename T, typename... Args>
    requires std::is_base_of_v<Entity, T> and std::is_constructible_v<T, Args...>
    static T& create(Args&&... args);

    /// @brief Destroy Entity / make invalid
    void destroy();

    /// @brief Emplace a component onto this Entity
    /// @tparam T Component type
    /// @tparam ...Args construct args
    /// @param ...args forwarded to component constructor
    /// @return reference to emplaced component
    template <typename T, typename... Args>
    requires std::is_constructible_v<T, Args...>
    T& emplace(Args&&... args);

    /// @brief Emplace a component onto this Entity or replace it with this new one
    /// @tparam T Component type
    /// @tparam ...Args construct args
    /// @param ...args forwarded to component constructor
    /// @return reference to component
    template <typename T, typename... Args>
    requires std::is_constructible_v<T, Args...>
    T& emplaceOrReplace(Args&&... args);

    /// @brief Replace a component on this entity
    /// @tparam T Component type
    /// @tparam ...Args constructor args
    /// @param ...args forwarded to component constructor
    /// @return a reference to the component being replaced
    template <typename T, typename... Args>
    requires std::is_constructible_v<T, Args...>
    T& replace(Args&&... args);

    /// @brief Remove a component from entity
    /// @tparam T component to remove
    /// @tparam ...Other variadic list of many components to remove
    /// @return the number of components removed from the Entity
    template <typename T, typename... Other>
    usize remove();

    /// @brief Get a component or components for an entities
    /// @tparam ...T component types
    /// @return reference or tuple of returned components
    template <typename... T>
    [[nodiscard]] decltype(auto) get();

    /// @brief Get a component or components for an entities
    /// @tparam ...T component types
    /// @return reference or tuple of returned components
    template <typename... T>
    [[nodiscard]] decltype(auto) get() const;

    /// @brief Get a component or components for an entities
    /// @tparam ...T component types, T may or make not be attached to Entity
    /// @return pointer to components
    template <typename... T>
    [[nodiscard]] auto tryGet();

    /// @brief Get a component or components for an entities
    /// @tparam ...T component types, T may or make not be attached to Entity
    /// @return pointer to components
    template <typename... T>
    [[nodiscard]] auto tryGet() const;

private:
    entt::entity m_id = entt::null;
    Scene* m_parentScene = nullptr;
};

} // namespace R3

#include "core/Entity.ipp"
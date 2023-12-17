#pragma once
#include <entt/entt.hpp>
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
    Entity() = default;

    Entity(const Entity&) = delete; ///< @brief non-copyable

    /// @brief Moveable
    /// @param src
    Entity(Entity&& src) noexcept
        : m_id(src.m_id),
          m_parentScene(src.m_parentScene) {
        src.m_id = entt::null;
    }

    void operator=(const Entity&) = delete; ///< @brief non-copyable

    /// @brief Moveable
    /// @param src
    /// @return Moved Entity
    Entity& operator=(Entity&& src) noexcept {
        m_id = src.m_id;
        m_parentScene = src.m_parentScene;
        src.m_id = entt::null;
        return *this;
    }

    /// @brief Get the Entity's owning scene
    /// @return parent scene
    Scene* parentScene() { return m_parentScene; }

    /// @brief Query whether or not the Entity is valid in the Registry.
    /// @return validity
    [[nodiscard]] bool valid() const;

    /// @brief Create an Entity class/subclass bound to a controling scene.
    /// @tparam T Entity type
    /// @tparam ...Args constructor args
    /// @param parentScene the scene to bind the entity
    /// @param ...args forwarded to the Entity-Derived constructor
    /// @return reference to created entity
    template <typename T, typename... Args>
    static T& create(Scene* parentScene, Args&&... args);

    /// @brief Destroy Entity / make invalid
    void destroy();

    /// @brief Emplace a component onto this Entity
    /// @tparam T Component type
    /// @tparam ...Args construct args
    /// @param ...args forwarded to component constructor
    /// @return reference to emplaced component
    template <typename T, typename... Args>
    T& emplace(Args&&... args);

    /// @brief Emplace a component onto this Entity or replace it with this new one
    /// @tparam T Component type
    /// @tparam ...Args construct args
    /// @param ...args forwarded to component constructor
    /// @return reference to component
    template <typename T, typename... Args>
    T& emplaceOrReplace(Args&&... args);

    /// @brief Replace a component on this entity
    /// @tparam T Component type
    /// @tparam ...Args constructor args
    /// @param ...args forwarded to component constructor
    /// @return a reference to the component being replaced
    template <typename T, typename... Args>
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
    entt::entity m_id{entt::null};
    Scene* m_parentScene{nullptr};
};

inline bool Entity::valid() const {
    return m_parentScene->m_registry.valid(m_id);
}

template <typename T, typename... Args>
inline T& Entity::create(Scene* parentScene, Args&&... args) {
    static_assert(std::is_base_of_v<Entity, T>);
    // add to registry
    entt::entity id = parentScene->m_registry.create();

    // add Entity derived component, what the api thinks of as 'an entity'
    T& entity = parentScene->m_registry.emplace<T>(id, std::forward<Args>(args)...);

    // setup fields
    entity.m_id = id;
    entity.m_parentScene = parentScene;

    // if entity is has init() method call it
    if constexpr (Initializable<T>) {
        entity.init();
    }

    // if entity has tick(double) add TickSystem (Scene ensures no duplicate systems)
    if constexpr (Tickable<T>) {
        Engine::activeScene().addSystem<TickSystem<T>>();
    }

    return entity;
}

inline void Entity::destroy() {
    m_parentScene->m_registry.destroy(m_id);
}

template <typename T, typename... Args>
inline T& Entity::emplace(Args&&... args) {
    return m_parentScene->m_registry.emplace<T>(m_id, std::forward<Args>(args)...);
}

template <typename T, typename... Args>
inline T& Entity::emplaceOrReplace(Args&&... args) {
    return m_parentScene->m_registry.emplace_or_replace<T>(m_id, std::forward<Args>(args)...);
}

template <typename T, typename... Args>
inline T& Entity::replace(Args&&... args) {
    return m_parentScene->m_registry.replace<T>(m_id, std::forward<Args>(args)...);
}

template <typename T, typename... Other>
inline usize Entity::remove() {
    return m_parentScene->m_registry.remove<T, Other...>(m_id);
}

template <typename... T>
inline decltype(auto) Entity::get() {
    return m_parentScene->m_registry.get<T...>(m_id);
}

template <typename... T>
inline decltype(auto) Entity::get() const {
    return m_parentScene->m_registry.get<T...>(m_id);
}

template <typename... T>
inline auto Entity::tryGet() {
    return m_parentScene->m_registry.try_get<T...>(m_id);
}

template <typename... T>
inline auto Entity::tryGet() const {
    return m_parentScene->m_registry.try_get<T...>(m_id);
}

} // namespace R3
#pragma once

#include "core/Entity.hpp"

namespace R3 {

inline bool Entity::valid() const {
    return m_parentScene->m_registry.valid(m_id);
}

template <typename T, typename... Args>
requires std::is_base_of_v<Entity, T> and std::is_constructible_v<T, Args...>
inline T& Entity::create(Args&&... args) {
    // add to registry
    entt::entity id = CurrentScene->m_registry.create();

    // add Entity derived component, what the api thinks of as 'an entity'
    T& entity = CurrentScene->m_registry.emplace<T>(id, std::forward<Args>(args)...);

    // every Entity gets a transform
    CurrentScene->m_registry.emplace<TransformComponent>(id, mat4(1.0f));

    // setup fields
    entity.m_id = id;
    entity.m_parentScene = CurrentScene;

    // if entity is has init() method call it
    if constexpr (Initializable<T>) {
        entity.init();
    }

    // if entity has tick(double) add TickSystem (Scene ensures no duplicate systems)
    if constexpr (Tickable<T>) {
        CurrentScene->addSystem<TickSystem<T>>();
    }

    return entity;
}

inline void Entity::destroy() {
    CHECK(valid());
    m_parentScene->m_registry.destroy(m_id);
}

template <typename T, typename... Args>
requires std::is_constructible_v<T, Args...>
inline T& Entity::emplace(Args&&... args) {
    static_assert(std::is_same_v<TransformComponent, T>, "Entities are already initialized with a Transform");
    return m_parentScene->m_registry.emplace<T>(m_id, std::forward<Args>(args)...);
}

template <typename T, typename... Args>
requires std::is_constructible_v<T, Args...>
inline T& Entity::emplaceOrReplace(Args&&... args) {
    return m_parentScene->m_registry.emplace_or_replace<T>(m_id, std::forward<Args>(args)...);
}

template <typename T, typename... Args>
requires std::is_constructible_v<T, Args...>
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
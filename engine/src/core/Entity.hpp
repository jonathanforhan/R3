#pragma once
#include <set>
#include <entt/entt.hpp>
#include "core/Scene.hpp"
#include "core/SubSystem.hpp"

namespace R3 {

class Entity {
protected:
    Entity() = default;

public:
    Entity(const Entity&) = delete;
    Entity(Entity&& src) noexcept
        : m_id(src.m_id),
          m_parentScene(src.m_parentScene) {
        src.m_id = entt::null;
    }
    void operator=(const Entity&) = delete;
    Entity& operator=(Entity&& src) noexcept {
        m_id = src.m_id;
        m_parentScene = src.m_parentScene;
        src.m_id = entt::null;
        return *this;
    }
    virtual ~Entity() {}

    virtual void tick(double){};

    [[nodiscard]] auto valid() -> bool const;

    template <typename T, typename... Args>
    [[nodiscard]] static auto create(Scene* parentScene, Args&&... args) -> T&;

    void destroy();

    template <typename T, typename... Args>
    auto emplace(Args&&... args) -> T&;

    template <typename T, typename... Args>
    auto emplaceOrReplace(Args&&... args) -> T&;

    template <typename T, typename... Args>
    auto replace(Args&&... args) -> usize;

    template <typename T, typename... Other>
    auto remove() -> usize;

    template <typename T, typename... Other>
    void erase();

    template <typename F>
    void eraseIf(F func);

    template <typename... T>
    void compact();

    template <typename... T>
    [[nodiscard]] decltype(auto) get();

    template <typename... T>
    [[nodiscard]] decltype(auto) get() const;

    template <typename... T>
    [[nodiscard]] auto tryGet();

    template <typename... T>
    [[nodiscard]] auto tryGet() const;

private:
    entt::entity m_id{entt::null};
    Scene* m_parentScene{nullptr};
};

template <typename T, typename... Args>
inline auto Entity::create(Scene* parentScene, Args&&... args) -> T& {
    static_assert(std::is_base_of_v<Entity, T>);
    // add to registry
    entt::entity id = parentScene->m_registry.create();
    // add Entity derived component, what the api thinks of as 'an entity'
    T& entity = parentScene->m_registry.emplace<T>(id, std::forward<Args>(args)...);
    // add subsystem
    if (!parentScene->m_system_set.contains(typeid(T).name())) {
        Engine::activeScene().addSystem<EntitySubSystem<T>>();
        parentScene->m_system_set.emplace(typeid(T).name());
    }
    // setup fields
    entity.m_id = id;
    entity.m_parentScene = parentScene;
    return entity;
}

inline auto Entity::valid() -> bool const {
    return m_parentScene->m_registry.valid(m_id);
}

inline void Entity::destroy() {
    m_parentScene->m_registry.destroy(m_id);
}

template <typename T, typename... Args>
inline auto Entity::emplace(Args&&... args) -> T& {
    return m_parentScene->m_registry.emplace<T>(m_id, std::forward<Args>(args)...);
}

template <typename T, typename... Args>
inline auto Entity::emplaceOrReplace(Args&&... args) -> T& {
    return m_parentScene->m_registry.emplace<T>(m_id, std::forward<Args>(args)...);
}

template <typename T, typename... Args>
inline auto Entity::replace(Args&&... args) -> usize {
    return m_parentScene->m_registry.replace<T>(m_id, std::forward<Args>(args)...);
}

template <typename T, typename... Other>
inline auto Entity::remove() -> usize {
    return m_parentScene->m_registry.replace<T, Other...>(m_id);
}

template <typename T, typename... Other>
inline void Entity::erase() {
    m_parentScene->m_registry.erase<T, Other...>(m_id);
}

template <typename F>
inline void Entity::eraseIf(F func) {
    m_parentScene->m_registry.erase_if<F>(m_id);
}

template <typename... T>
inline void Entity::compact() {
    m_parentScene->m_registry.compact<T...>(m_id);
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
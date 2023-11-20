#pragma once
#include <entt/entt.hpp>
#include "core/Scene.hpp"
#include "systems/TickSystem.hpp"

namespace R3 {

//! used to check if we can/must initialize an Entity
template <typename T>
concept Initializable = requires(T t) { t.init(); };

/*!
 * \brief Entity base class that other Entity subclasses can derive
 *
 * when an entity is created with Entity::create<Derived> *if* it is Tickable
 * (implements tick(double) method [checked through a C++20 concept]) then
 * we add a TickSystem for the entity and it's tick function gets called every frame
 * there is no virtual function overhead with the tick system just a normal c++ method call
 * if the class is `Initializable` (implements init() method) that will be called as well
 */
class Entity {
public:
    Entity() = default;
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

    //! get the Entities owning scene
    Scene* parentScene() { return m_parentScene; }

    //! query whether or not the Entity is valid in the Registry
    [[nodiscard]] bool valid() const;

    /*!
     * \brief create an Entity class/subclass bound to a controling scene
     * \param parentScene the scene to bind the entity
     * \param args forwarded to the Entity-Derived constructor
     */
    template <typename T, typename... Args>
    static T& create(Scene* parentScene, Args&&... args);

    void destroy();

    template <typename T, typename... Args>
    T& emplace(Args&&... args);

    template <typename T, typename... Args>
    T& emplaceOrReplace(Args&&... args);

    template <typename T, typename... Args>
    usize replace(Args&&... args);

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
    return m_parentScene->m_registry.emplace<T>(m_id, std::forward<Args>(args)...);
}

template <typename T, typename... Args>
inline usize Entity::replace(Args&&... args) {
    return m_parentScene->m_registry.replace<T>(m_id, std::forward<Args>(args)...);
}

template <typename T, typename... Other>
inline usize Entity::remove() {
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
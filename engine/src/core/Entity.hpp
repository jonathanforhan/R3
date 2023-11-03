#pragma once
#include "api/Types.hpp"
#include "core/Scene.hpp"
#include "core/EntityBase.hpp"

namespace R3 {

// Every Entity Derives from Entity
// The Entity class is a universal component on every entt::entity that gets created
// it provides basic management methods and a tick() method that will get called every gametick
class Entity : public EntityBase {
protected:
    Entity() = default;

public:
    Entity(const Entity&) = delete;
    Entity(Entity&&) = default;
    void operator=(const Entity&) = delete;
    Entity& operator=(Entity&&) = default;
    virtual ~Entity() {}

    virtual void tick(double) = 0;

    // returns whether the Entity is in a valid state
    [[nodiscard]] bool valid() const {
        return Scene::instance()._registery.valid(_id);
    }

    // creates a registered entt::entity with a T component on it, T must extend Entity
    template <typename T, typename... Args>
    [[nodiscard]] static T& create(Args&&... args) {
        static_assert(std::is_base_of_v<Entity, T>);
        entt::entity id = (Scene::instance()._registery.create());
        T& entity = Scene::instance()._registery.emplace<T>(id, std::forward<Args>(args)...);
        entity._id = id;
        return entity;
    }

    // destroys the entt registery of the Entity and destoys it's components
    void destroy() {
        Scene::instance()._registery.destroy(_id);
    }

    // emplace a component in entity, if component is already attached it's UB
    template <typename T, typename... Args>
    T& emplace(Args&&... args) {
        return Scene::instance()._registery.emplace<T>(_id, std::forward<Args>(args)...);
    }

    // emplace or replace an existing component
    template <typename T, typename... Args>
    T& emplace_or_replace(Args&&... args) {
        return Scene::instance()._registery.emplace_or_replace<T>(_id, std::forward<Args>(args)...);
    }

    // replace component
    template <typename T, typename... Args>
    T& replace(Args&&... args) {
        return Scene::instance()._registery.replace<T>(_id, std::forward<Args>(args)...);
    }

    template<typename T, typename... Other>
    usize remove() {
        return Scene::instance()._registery.remove<T>(_id);
    }

    template<typename T, typename... Other>
    void erase() {
        return Scene::instance()._registery.remove<T, Other>(_id);
    }

    template<typename Func>
    void erase_if(Func func) {
        return Scene::instance()._registery.erase_if<Func>(_id);
    }
    template<typename... T>
    void compact() {
        return Scene::instance()._registery.compact<T>(_id);
    }

    // get entity's component
    template<typename... T>
    [[nodiscard]] decltype(auto) get() {
        return Scene::instance()._registery.get<T...>(_id);
    }

    template<typename... T>
    [[nodiscard]] decltype(auto) get() const {
        return Scene::instance()._registery.get<T...>(_id);
    }

    template<typename... T>
    [[nodiscard]] auto try_get() {
        return Scene::instance()._registery.try_get<T...>(_id);
    }

    template<typename... T>
    [[nodiscard]] auto try_get() const {
        return Scene::instance()._registery.try_get<T...>(_id);
    }
};

} // namespace R3
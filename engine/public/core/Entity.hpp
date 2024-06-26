#pragma once

/// @file Entity.hpp
/// @brief Provides the Entity part of the Entity Component System

#include <R3>
#include <entt/entt.hpp>
#include "components/TransformComponent.hpp"
#include "core/Scene.hpp"

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
class R3_API Entity {
public:
    template <typename... T>
    using Exclude = entt::exclude_t<T...>;

    DEFAULT_CONSTRUCT(Entity);
    NO_COPY(Entity);
    DEFAULT_MOVE(Entity);

    /// @brief Get Entity ID
    /// @return id
    uuid32 id() const { return uuid32(m_id); }

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

    /// @brief Iterate over all the entities in current Scene
    /// Pass this a lambda with an EntityView ref as the argument you would like to iterate through as arguments
    /// @code
    /// Entity::forEach([](EntityView& ev){ LOG(Info, ev.id()); });
    /// @endcode
    /// @param callback
    template <typename F>
    requires requires(F&& f) { f(std::declval<EntityView&>()); }
    static void forEach(F&& callback, Scene* scene = nullptr);

    /// @brief Query Component View from entt of Current Scene
    /// @tparam ...T Type of Components to Query
    /// @tparam ...Exclude Components to exclude
    /// @return Component View
    template <typename... T, typename... Ex>
    [[nodiscard]] static decltype(auto) componentView(Exclude<Ex...> = entt::exclude_t{});

#if not R3_ENGINE
    /// @brief Iterate Components from View of Current Scene
    /// This will use the FunctionTraits<F> type deduction, Limited to four Components
    /// Pass the components you would like to iterate as arguements
    /// @code
    /// Entity::forEachComponent([](TransformComponent& t){ t = mat4(1.0f); });
    /// @endcode
    /// @tparam F Functor
    /// @param callback
    template <typename F>
    requires requires { FunctionTraits<F>::Arity::value <= 4; }
    static void forEachComponent(F&& callback);
#endif

protected:
    entt::entity m_id = entt::null;
    Scene* m_parentScene = nullptr;
};

class EntityView : public Entity {
public:
    DEFAULT_COPY(EntityView);
    NO_MOVE(EntityView);

    EntityView(uuid32 id, Scene* parentScene)
        : Entity() {
        m_id = entt::entity(id);
        m_parentScene = parentScene;
    }
};

} // namespace R3

#include "core/Entity.ipp"

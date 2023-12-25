#pragma once

/// @file Scene.hpp
/// @brief Provid Scene control for the API

#include <entt/entt.hpp>
#include <unordered_map>
#include "api/Types.hpp"
#include "core/Engine.hpp"
#include "systems/System.hpp"

namespace R3 {

/// @brief A Scene handle Events and has an ECS Registry ties to it
class Scene {
public:
    /// @brief Create Scene with uuid
    /// @param id uuid
    explicit Scene(uuid32 id);

    /// @brief Query Component View from entt
    /// @tparam ...T Type of Components to Query
    /// @return Component View
    template <typename... T>
    [[nodiscard]] static decltype(auto) componentView();

    /// @brief Iterate Components from View
    /// This will use the FunctionTraits<F> type deduction
    /// Limited to two Components
    /// @tparam F Functor
    /// @param callback
    template <typename F>
    requires requires { FunctionTraits<F>::Arity::value <= 2; }
    static void componentForEach(F&& callback);

    /// @brief Add a System to the Scene, will only add if not already present
    /// @tparam T System
    /// @tparam ...Args Constructor Args
    /// @param ...args
    template <typename T, typename... Args>
    requires ValidSystem<T, Args...>
    static void addSystem(Args&&... args);

    /// @brief Push an Event onto the Event queue where is can be called.
    /// Events get freed from heap memory when they are dispatched
    template <typename Event, typename... Args>
    requires std::is_constructible_v<typename Event::PayloadType, Args...>
    static constexpr void pushEvent(Args&&... args);

    /// @brief Pope event from queue, is freed automatically
    static void popEvent();

    /// @brief Query Top event
    /// used to not load a bunch of redundant events like in Window Resize
    /// @return Top Event ID
    static uuid32 topEvent();

    /// @brief Bind an Event listener to the current Scene
    /// Uses FunctionTraits type deduction to deduce the Event
    /// @tparam F Functor
    /// @param callback
    template <typename F>
    requires EventListener<F>
    static void bindEventListener(F&& callback);

    /// @brief Set View Matrix of Entire Scene
    /// Used by CameraSystem
    /// @param view
    static void setView(const mat4& view);

    /// @brief Set Projection Matrix of Entire Scene
    /// Used by CameraSystem
    /// @param projection
    static void setProjection(const mat4& projection);

    /// @brief Set Camera Position
    /// @return position
    static void setCameraPosition(vec3 position);

    /// @brief Query View Matrix
    /// @return view
    static const mat4& view();

    /// @brief Query Projection Matrix
    /// @return projection
    static const mat4& projection();

    /// @brief Query Camera Position
    /// @return position
    static vec3 cameraPosition();

private:
    void runSystems(double dt);

public:
    const uuid32 id; ///< Scene id

private:
    //--- Graphics
    mat4 m_view = mat4(1.0f);
    mat4 m_projection = mat4(1.0f);
    vec3 m_cameraPosition = vec3(0.0f);

    //--- ECS
    entt::registry m_registry;
    std::vector<std::unique_ptr<System>> m_systems;
    std::set<std::string> m_systemSet;

    //--- Event System
    std::queue<std::span<std::byte>> m_eventQueue;                   // tracks event byte array
    std::vector<std::byte> m_eventArena;                             // memory pool for allocations when pushing events
    std::unordered_multimap<uuid32, EventCallback> m_eventRegistery; // mapping of id to callback

    //--- Accessors
    // Entity is a friend class to access the registry
    // a lot of entity management is done through Entity
    // this provides cleaner api at cost of being non-local
    friend class Entity;
    friend class Engine;
};

} // namespace R3

#include "core/Scene.ipp"
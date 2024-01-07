#pragma once

#include <R3>
#include <entt/entt.hpp>
#include <queue>
#include "core/Engine.hpp"
#include "input/Event.hpp"
#include "systems/System.hpp"

namespace R3 {

class EntityView;

class R3_API Scene {
public:
    using ResourceManagerImpl = void*;

    /// @brief Create Scene with uuid
    /// @param id uuid
    explicit Scene(uuid32 id, const char* name);

    /// @brief clear the Scene's ECS Registry of all entities and components
    void clearRegistry();

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

    /// @brief Query whether the event queues holds any events
    /// @return t/f
    static bool isEventQueueEmpty();

#if not R3_ENGINE
    /// @brief Bind an Event listener to the current Scene
    /// Uses FunctionTraits type deduction to deduce the Event
    /// @tparam F Functor
    /// @param callback
    template <typename F>
    requires EventListener<F>
    static void bindEventListener(F&& callback);
#endif

    /// @brief Bind an Event listener to the current Scene
    /// Uses std::function so that it can be dll exported
    /// @tparam F Functor
    /// @param callback
    template <typename E>
    static void bindEventListener(std::function<void(const E&)> callback);

#if R3_ENGINE
    /// @brief Dispath the all the events callbacks in the queue
    static void dispatchEvents();

    /// @brief Run the systems bound to scene
    /// @param dt
    static void runSystems(double dt);
#endif

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

    /// @brief Set Cursor Position (normalized)
    /// @return position
    static void setCursorPosition(vec2 position);

    /// @brief Query View Matrix
    /// @return view
    [[nodiscard]] static const mat4& view();

    /// @brief Query Projection Matrix
    /// @return projection
    [[nodiscard]] static const mat4& projection();

    /// @brief Query Camera Position
    /// @return position
    [[nodiscard]] static vec3 cameraPosition();

    /// @brief Query Cursor Position
    /// @return position
    [[nodiscard]] static vec2 cursorPosition();

public:
    const uuid32 id;                     ///< Scene id
    const char* name;                    ///< Scene name
    ResourceManagerImpl resourceManager; ///< Handle to Scene ResourceManager

private:
    //--- Graphics
    mat4 m_view = mat4(1.0f);
    mat4 m_projection = mat4(1.0f);
    vec3 m_cameraPosition = vec3(0.0f);
    vec2 m_cursorPosition = vec2(0);

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
};

} // namespace R3

#include "core/Scene.ipp"

#pragma once

#include <entt/entt.hpp>
#include "api/Types.hpp"
#include "core/Engine.hpp"
#include "systems/System.hpp"

namespace R3 {

class Scene {
public:
    explicit Scene(uuid32 id);

    template <typename... T>
    [[nodiscard]] static decltype(auto) componentView();

    template <typename T, typename... Args>
    requires ValidSystem<T, Args...>
    static void addSystem(Args&&... args);

    /// @brief Push an Event onto the Event queue where is can be called.
    /// Events get freed from heap memory when they are dispatched. There is a static_assert
    /// on Event ensuring that the Event is trivially deconstructable so we can delete the void*
    template <typename Event, typename... Args>
    requires requires(Args&&... args) { std::is_constructible_v<typename Event::PayloadType, Args...>; }
    static constexpr void pushEvent(Args&&... args);

    static void popEvent();

    static uuid32 topEvent();

    template <typename F>
    requires EventListener<F>
    static constexpr void bindEventListener(F&& callback);

    static void setView(const mat4& view);
    static void setProjection(const mat4& projection);
    static mat4& view();
    static mat4& projection();

private:
    void runSystems(double dt);

public:
    const uuid32 id;

private:
    //--- Graphics
    mat4 m_view = mat4(1.0f);
    mat4 m_projection = mat4(1.0f);

    //--- ECS
    entt::registry m_registry;
    std::vector<std::unique_ptr<System>> m_systems;
    std::set<std::string> m_systemSet;

    //--- Event System
    std::queue<std::pair<void*, usize>> m_eventQueue;                // tracks event (as void*) and the payload size
    std::vector<uint8> m_eventArena;                                 // memory pool for allocations when pushing events
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
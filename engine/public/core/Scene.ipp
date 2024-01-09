#include "core/Scene.hpp"

namespace R3 {

template <typename T, typename... Args>
requires ValidSystem<T, Args...>
inline void Scene::addSystem(Args&&... args) {
    auto& systemSet = CurrentScene->m_systemSet;
    auto& systems = CurrentScene->m_systems;

    if (!systemSet.contains(typeid(T).name())) {
        systems.emplace_back(new T(std::forward<Args>(args)...));
        systemSet.emplace(typeid(T).name());
    }
}

template <typename Event, typename... Args>
requires std::is_constructible_v<typename Event::PayloadType, Args...>
inline constexpr void Scene::pushEvent(Args&&... args) {
    auto& eventArena = CurrentScene->m_eventArena;
    auto& eventQueue = CurrentScene->m_eventQueue;

    using Payload_T = typename Event::PayloadType;
    const auto event = Event(Payload_T(std::forward<Args>(args)...));        // construct event
    const usize iEnd = eventArena.size();                                    // get the offset
    eventArena.resize(eventArena.size() + sizeof(event));                    // resize our arena to fit event
    memcpy(&eventArena[iEnd], &event, sizeof(event));                        // memcpy the event to arena
    eventQueue.push(std::span<std::byte>(&eventArena[iEnd], sizeof(event))); // add payload addr and size to queue
}

inline void Scene::clearRegistry() {
    m_registry.clear();
}

inline void Scene::popEvent() {
    auto& eventArena = CurrentScene->m_eventArena;
    auto& eventQueue = CurrentScene->m_eventQueue;

    if (!eventQueue.empty()) {
        const usize eventSize = eventQueue.front().size();
        eventArena.resize(eventArena.size() - eventSize);
        eventQueue.pop();
    }
}

inline uuid32 Scene::topEvent() {
    auto& eventQueue = CurrentScene->m_eventQueue;
    return eventQueue.empty() ? 0 : *(uuid32*)eventQueue.front().data();
}

inline bool Scene::isEventQueueEmpty() {
    return CurrentScene->m_eventQueue.empty();
}

#if not R3_ENGINE
template <typename F>
requires EventListener<F>
inline void Scene::bindEventListener(F&& callback) {
    auto& eventRegistery = CurrentScene->m_eventRegistery;
    using Event_T = EventTypeDeduced<F>;
    // create wrapper so that we can store functions with void ptr param in registry
    EventCallback wrapper = [callback](const void* event) { callback((const Event_T&)(*(const Event_T*)event)); };
    eventRegistery.emplace(Event_T::SingalType::value, wrapper);
}
#endif

template <typename E, typename F>
inline void Scene::bindEventListener(F&& callback) {
    auto& eventRegistery = CurrentScene->m_eventRegistery;
    // create wrapper so that we can store functions with void ptr param in registry
    EventCallback wrapper = [callback](const void* event) { callback((const E&)(*(const E*)event)); };
    eventRegistery.emplace(E::SingalType::value, wrapper);
}

#if R3_ENGINE
inline void Scene::dispatchEvents() {
    while (!Scene::isEventQueueEmpty()) {
        void* const p = CurrentScene->m_eventQueue.front().data();
        const uuid32 id = *(uuid32*)p;

        auto range = CurrentScene->m_eventRegistery.equal_range(id);
        for (auto& it = range.first; it != range.second; ++it) {
            it->second(p);
        }

        Scene::popEvent();
    }
}

inline void Scene::runSystems(double dt) {
    // tick our systems
    for (const auto& system : CurrentScene->m_systems) {
        system->tick(dt);
    }
}
#endif

inline void Scene::setView(const mat4& view) {
    CurrentScene->m_view = view;
}

inline void Scene::setProjection(const mat4& projection) {
    CurrentScene->m_projection = projection;
}

inline void Scene::setCameraPosition(vec3 position) {
    CurrentScene->m_cameraPosition = position;
}

inline void Scene::setCursorPosition(vec2 position) {
    CurrentScene->m_cursorPosition = position;
}

inline const mat4& Scene::view() {
    return CurrentScene->m_view;
}

inline const mat4& Scene::projection() {
    return CurrentScene->m_projection;
}

inline vec3 Scene::cameraPosition() {
    return CurrentScene->m_cameraPosition;
}

inline vec2 Scene::cursorPosition() {
    return CurrentScene->m_cursorPosition;
}

} // namespace R3
#include "core/Scene.hpp"

namespace R3 {

template <typename... T>
inline decltype(auto) Scene::componentView() {
    return Engine::activeScene().m_registry.view<T...>();
}

template <typename F>
requires requires { FunctionTraits<F>::Arity::value <= 2; }
inline void Scene::componentForEach(F&& callback) {
    using Traits = FunctionTraits<F>;

    static_assert(Traits::Arity::value != 0, "need a component to iterate through");

    if constexpr (Traits::Arity::value == 1) {
        using Arg0 = std::remove_reference_t<typename Traits::template ArgType<0>>;
        Engine::activeScene().m_registry.view<Arg0>().each(callback);
    } else if constexpr (Traits::Arity::value == 2) {
        using Arg0 = std::remove_reference_t<typename Traits::template ArgType<0>>;
        using Arg1 = std::remove_reference_t<typename Traits::template ArgType<1>>;
        Engine::activeScene().m_registry.view<Arg0, Arg1>().each(callback);
    }
}

template <typename T, typename... Args>
requires ValidSystem<T, Args...>
inline void Scene::addSystem(Args&&... args) {
    auto& systemSet = Engine::activeScene().m_systemSet;
    auto& systems = Engine::activeScene().m_systems;

    if (!systemSet.contains(typeid(T).name())) {
        systems.emplace_back(new T(std::forward<Args>(args)...));
        systemSet.emplace(typeid(T).name());
    }
}

template <typename Event, typename... Args>
requires std::is_constructible_v<typename Event::PayloadType, Args...>
inline constexpr void Scene::pushEvent(Args&&... args) {
    auto& eventArena = Engine::activeScene().m_eventArena;
    auto& eventQueue = Engine::activeScene().m_eventQueue;

    using Payload_T = typename Event::PayloadType;
    const auto event = Event(Payload_T(std::forward<Args>(args)...));        // construct event
    const usize iEnd = eventArena.size();                                    // get the offset
    eventArena.resize(eventArena.size() + sizeof(event));                    // resize our arena to fit event
    memcpy(&eventArena[iEnd], &event, sizeof(event));                        // memcpy the event to arena
    eventQueue.push(std::span<std::byte>(&eventArena[iEnd], sizeof(event))); // add payload addr and size to queue
}

inline void Scene::popEvent() {
    auto& eventArena = Engine::activeScene().m_eventArena;
    auto& eventQueue = Engine::activeScene().m_eventQueue;

    if (!eventQueue.empty()) {
        const usize eventSize = eventQueue.front().size();
        eventArena.resize(eventArena.size() - eventSize);
        eventQueue.pop();
    }
}

inline uuid32 Scene::topEvent() {
    auto& eventQueue = Engine::activeScene().m_eventQueue;
    return eventQueue.empty() ? 0 : *(uuid32*)eventQueue.front().data();
}

template <typename F>
requires EventListener<F>
inline void Scene::bindEventListener(F&& callback) {
    auto& eventRegistery = Engine::activeScene().m_eventRegistery;
    using Event_T = EventTypeDeduced<F>;
    // create wrapper so that we can store functions with void ptr param in registry
    EventCallback wrapper = [callback](const void* event) { callback((const Event_T&)(*(const Event_T*)event)); };
    eventRegistery.insert(std::make_pair(Event_T::SingalType::value, wrapper));
}

inline void Scene::setView(const mat4& view) {
    Engine::activeScene().m_view = view;
}

inline void Scene::setProjection(const mat4& projection) {
    Engine::activeScene().m_projection = projection;
}

inline mat4& Scene::view() {
    return Engine::activeScene().m_view;
}

inline mat4& Scene::projection() {
    return Engine::activeScene().m_projection;
}

inline void Scene::runSystems(double dt) {
    // tick our systems
    for (const auto& system : m_systems) {
        system->tick(dt);
    }
}

} // namespace R3
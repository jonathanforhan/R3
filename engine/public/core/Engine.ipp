#pragma once

#include "core/Engine.hpp"

namespace R3 {

template <typename Event, typename... Args>
inline constexpr void Engine::pushEvent(Args&&... args) {
    Engine::inst().pushEventHelper<Event>(std::forward<Args>(args)...);
}

inline void Engine::popEvent() {
    Engine::inst().popEventHelper();
}

inline uuid32 Engine::topEvent() {
    auto& engine = Engine::inst();
    return engine.m_eventQueue.empty() ? 0 : *(uuid32*)engine.m_eventQueue.front().first;
}

template <typename F>
inline constexpr void Engine::bindEventListener(F&& callback) {
    Engine::inst().bindEventListenerHelper(std::forward<F>(callback));
}

template <typename Event, typename... Args>
inline void Engine::pushEventHelper(Args&&... args) {
    using Payload_T = typename Event::template PayloadType;
    auto event = Event(Payload_T(std::forward<Args>(args)...));                // construct event
    usize iEnd = m_eventArena.size();                                          // get the offset
    m_eventArena.resize(m_eventArena.size() + sizeof(event));                  // resize our arena to fit event
    memcpy(&m_eventArena[iEnd], &event, sizeof(event));                        // memcpy the event to arena
    m_eventQueue.push(std::make_pair(&m_eventArena[iEnd], sizeof(Payload_T))); // add payload addr & size to queue
}

inline void Engine::popEventHelper() {
    if (!m_eventQueue.empty()) {
        auto payloadSize = m_eventQueue.front().second;
        m_eventArena.resize(m_eventArena.size() - payloadSize);
        m_eventQueue.pop();
    }
}

template <typename F>
inline void Engine::bindEventListenerHelper(F&& callback) {
    using traits = FunctionTraits<F>;
    using EventRef_T = typename traits::template ArgType<0>;

    // sanity check
    static_assert(std::is_reference_v<EventRef_T> and std::is_same_v<typename traits::ResultType, void> and
                      std::is_same_v<typename traits::template Arity, std::integral_constant<std::size_t, 1>>,
                  "callback structure: [...](Event<Signal, Payload>& event) -> void {...})");

    using Event_T = std::remove_reference_t<EventRef_T>;
    // create wrapper so that we can store functions with void ptr param in registry
    std::function<void(void*)> wrapper = [callback](void* event) { callback(*(Event_T*)event); };
    this->m_eventRegistery.insert(std::make_pair(Event_T::type, wrapper));
}

} // namespace R3
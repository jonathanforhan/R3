#pragma once

#include "core/Engine.hpp"

namespace R3 {

template <typename Event, typename... Args>
requires requires(Args&&... args) { std::is_constructible_v<typename Event::PayloadType, Args...>; }
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
requires EventListener<F>
inline constexpr void Engine::bindEventListener(F&& callback) {
    Engine::inst().bindEventListenerHelper(std::forward<F>(callback));
}

template <typename Event, typename... Args>
requires requires(Args&&... args) { std::is_constructible_v<typename Event::PayloadType, Args...>; }
inline void Engine::pushEventHelper(Args&&... args) {
    using Payload_T = typename Event::PayloadType;
    const auto event = Event(Payload_T(std::forward<Args>(args)...));      // construct event
    const usize iEnd = m_eventArena.size();                                // get the offset
    m_eventArena.resize(m_eventArena.size() + sizeof(event));              // resize our arena to fit event
    memcpy(&m_eventArena[iEnd], &event, sizeof(event));                    // memcpy the event to arena
    m_eventQueue.push(std::make_pair(&m_eventArena[iEnd], sizeof(event))); // add payload addr and size to queue
}

inline void Engine::popEventHelper() {
    if (!m_eventQueue.empty()) {
        auto eventSize = m_eventQueue.front().second;
        m_eventArena.resize(m_eventArena.size() - eventSize);
        m_eventQueue.pop();
    }
}

template <typename F>
requires EventListener<F>
inline void Engine::bindEventListenerHelper(F&& callback) {
    using Event_T = EventTypeDeduced<F>;
    // create wrapper so that we can store functions with void ptr param in registry
    std::function<void(void*)> wrapper = [callback](void* event) { callback(*(Event_T*)event); };
    this->m_eventRegistery.insert(std::make_pair(Event_T::template SingalType::value, wrapper));
}

} // namespace R3
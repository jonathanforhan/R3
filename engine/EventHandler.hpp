#pragma once

#include <cstddef>
#include <cstring>
#include <functional>
#include <list>
#include <queue>
#include <span>
#include <string_view>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <vector>
#include "FunctionTraits.hpp"
#include "Hash.hpp"
#include "Types.hpp"

namespace R3 {

/**
 * @brief Used to hash event-name string
 * @param str cstring pointer
 * @param len cstring lenght
 * @return unique id generated from your string
 */
consteval uint64 operator""_event(const char* str, usize len) {
    return hash::fnv1a(std::string_view{str, str + len});
}

template <typename Data>
requires std::is_trivial_v<Data>
struct Event final {
    using DataType = Data;

    /**
     * @brief Create an event by hashing eventName
     * @param id   hashed name of event e.g. "on-mouse-up"_event
     * @param data data to sent with event
     */
    constexpr Event(uint64 id, Data data)
        : id(id),
          data(data) {}

    const uint64 id; /** unique event id */
    const Data data; /** event data */
};

template <typename F>
using EventTypeDeduced = typename std::remove_reference_t<typename FunctionTraits<F>::template ArgType<0>>;

template <typename F>
concept EventListener = std::is_const_v<EventTypeDeduced<F>> and requires {
    typename std::enable_if_t<FunctionTraits<F>::Arity::value == 1>;                      // lambda has 1 param
    typename std::enable_if_t<offsetof(EventTypeDeduced<F>, id) == 0>;                    // event has id
    typename std::enable_if_t<std::is_trivial_v<typename EventTypeDeduced<F>::DataType>>; // event has data
};

class EventHandler {
private:
    EventHandler()  = default;
    ~EventHandler() = default;

public:
    EventHandler(const EventHandler&)            = delete;
    EventHandler& operator=(const EventHandler&) = delete;

    EventHandler(EventHandler&&) noexcept            = delete;
    EventHandler& operator=(EventHandler&&) noexcept = delete;

    using EventCallback = std::function<void(const void*)>;

    static EventHandler& instance() {
        static EventHandler instance;
        return instance;
    }

    template <typename Data>
    void pushEvent(uint64 id, const Data& data) {
        const Event event{id, data};
        usize i = m_eventArena.size();
        m_eventArena.resize(m_eventArena.size() + sizeof(event));
        uint8* end = &m_eventArena[i];
        std::memcpy(end, &event, sizeof(event));
        m_eventQueue.push(std::span<uint8>{end, end + sizeof(event)});
    }

    void popEvent() {
        if (!m_eventQueue.empty()) {
            const usize eventSize = m_eventQueue.front().size();
            m_eventArena.resize(m_eventArena.size() - eventSize);
            m_eventQueue.pop();
        }
    }

    template <typename F>
    requires EventListener<F>
    void bindEventListener(uint64 id, F callback) {
        using EventType       = EventTypeDeduced<F>;
        EventCallback wrapper = [callback](const void* event) { callback(*static_cast<const EventType*>(event)); };
        m_eventRegistry.insert(std::make_pair(id, wrapper));
    }

    /**
     * @brief Dispatch an event by processing it and calling listeners
     * @return the amount of events left in the queue
     */
    usize dispatchEvent() {
        if (!m_eventQueue.empty()) {
            void* const p   = m_eventQueue.front().data();
            const uint64 id = *(uint64* const)p;

            auto range = m_eventRegistry.equal_range(id);
            for (auto& it = range.first; it != range.second; ++it) {
                it->second(p);
            }
            popEvent();
        }
        return m_eventQueue.size();
    }

private:
    std::queue<std::span<uint8>> m_eventQueue;                      // tracks event byte array
    std::vector<uint8> m_eventArena;                                // memory pool for allocations when pushing events
    std::unordered_multimap<uint64, EventCallback> m_eventRegistry; // mapping id to callback
};

} // namespace R3
#pragma once

#include <cstddef>
#include <functional>
#include <initializer_list>
#include <list>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <vector>
#include "FunctionTraits.hpp"
#include "Hash.hpp"
#include "Memory.hpp"
#include "Types.hpp"

namespace R3 {

/**
 * @brief EventBase class is used as a virtual base for the event queue
 */
struct EventBase {
protected:
    constexpr EventBase(hash::uuid id) noexcept
        : id{id} {}

public:
    virtual ~EventBase() noexcept {}

    EventBase(const EventBase&)            = default;
    EventBase& operator=(const EventBase&) = default;

    EventBase(EventBase&&) noexcept            = default;
    EventBase& operator=(EventBase&&) noexcept = default;

    const hash::uuid id; /**< unique event id */
};

/**
 * @brief Derived Event which is templated for custom event data
 * @tparam Data Data stored in the event, this data will take up space on the queue so is should be small
 */
template <typename Data>
struct Event : public EventBase {
    using DataType = Data;

    constexpr Event(hash::uuid id, const Data& data) noexcept
        : EventBase{id},
          data{data} {}

    virtual ~Event() noexcept override {}

    Event(const Event&)            = default;
    Event& operator=(const Event&) = default;

    Event(Event&&) noexcept            = default;
    Event& operator=(Event&&) noexcept = default;

    const Data data; /**< event data */
};

template <typename F>
using EventTypeDeduced = typename std::remove_reference_t<typename FunctionTraits<F>::template ArgType<0>>;

template <typename F>
concept EventListener = std::is_const_v<EventTypeDeduced<F>> and requires {
    typename std::enable_if_t<FunctionTraits<F>::Arity::value == 1>;              // lambda has 1 param
    typename std::enable_if_t<std::is_base_of_v<EventBase, EventTypeDeduced<F>>>; // event is derived from EventBase
};

/**
 * @brief Singleton event handler which you can push event to and bind listeners to
 *
 * @code
 * EventHandler::instance().bindEventListener("key-press", [](const Event<KeyboardEventData>& e){
 *     LOG_INFO("key pressed: {}", (int)e.data.key);
 * });
 * @endcode
 */
class EventHandler {
private:
    using EventCallback = std::function<void(const EventBase&)>;

private:
    EventHandler() {
        m_eventQueue.reserve(10'000);
        m_eventArena.reserve(kilobyte(64));
    }

    ~EventHandler() noexcept { dispatchEvents(); };

public:
    EventHandler(const EventHandler&)            = delete;
    EventHandler& operator=(const EventHandler&) = delete;

    EventHandler(EventHandler&&) noexcept            = delete;
    EventHandler& operator=(EventHandler&&) noexcept = delete;

    /**
     * @brief Single EventHandler instance
     * @return global static instance of EventHandler
     */
    static EventHandler& instance() {
        static EventHandler instance;
        return instance;
    }

    /**
     * @brief Push an event onto the event queue
     * @tparam Data Event DataType
     * @param id    Event id e.g. "key-press"
     * @param data  Event data payload
     */
    template <typename Data>
    void push(hash::uuid id, const Data& data) {
        using EventType = Event<Data>;

        // calculate the next aligned offset
        const usize alignedOffset = getAlignedOffset<EventType>(m_eventArena.size());

        // resize arena to accommodate the aligned object
        m_eventArena.resize(alignedOffset + sizeof(EventType));

        // get properly aligned pointer
        void* alignedPtr = &m_eventArena[alignedOffset];

        // construct event in place
        new (alignedPtr) EventType{id, data};

        // push the event byte offset to queue
        m_eventQueue.emplace_back(alignedOffset);
    }

    /**
     * @brief Dispatch all event by calling every listener and then destructing the event objects
     */
    void dispatchEvents() {
        // iterate queued event offsets
        for (usize offset : m_eventQueue) {
            // interpret event from bytes
            EventBase* event = reinterpret_cast<EventBase*>(&m_eventArena[offset]);

            // make callback calls
            auto range = m_eventRegistry.equal_range(event->id);
            for (auto& it = range.first; it != range.second; ++it) {
                it->second(*event);
            }

            // manually destructor because EventHandler owns the lifetime
            event->~EventBase();
        }

        // all events are handled and all destructors called
        // can now safely overwrite memory
        m_eventQueue.clear();
        m_eventArena.clear();
    }

    /**
     * @brief Bind an event listener to listen for events that have the same id
     * @tparam F       Functor
     * @param id       event id to listen to
     * @param callback event callback triggered when id is emitted
     */
    template <typename F>
    requires EventListener<F>
    void bindEventListener(hash::uuid id, F callback) {
        using EventType       = EventTypeDeduced<F>;
        EventCallback wrapper = [callback](const EventBase& base) { callback(static_cast<const EventType&>(base)); };
        m_eventRegistry.insert(std::make_pair(id, wrapper));
    }

    template <typename F>
    requires EventListener<F>
    void bindEventListener(std::initializer_list<hash::uuid> ids, F callback) {
        for (auto& id : ids) {
            bindEventListener(id, callback);
        }
    }

private:
    template <typename T>
    constexpr usize getAlignedOffset(usize curr) {
        return (curr + alignof(T) - 1) & ~(alignof(T) - 1);
    }

private:
    std::vector<usize> m_eventQueue;                                // tracks event indices
    std::vector<std::byte> m_eventArena;                            // memory pool for allocations when pushing events
    std::unordered_multimap<uint64, EventCallback> m_eventRegistry; // mapping id to callback
};

} // namespace R3

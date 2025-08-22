/// @file EventHandler.hpp
/// @brief Event handling system
///
/// The EventHandler provides a centralized event dispatch system that uses compile-time type checking
/// to ensure event listeners are properly typed and exception-safe. Events are stored in a memory arena
/// for optimal performance and minimal heap fragmentation.
///
/// Basic Usage
///
/// 1. Define Custom Event Data
/// @code
/// struct PlayerMoveData {
///     float x, y;
///     int playerId;
/// };
/// @endcode
///
/// 2. Create Event Listeners
/// Event listeners must be noexcept lambdas that take a const Event<DataType>& parameter:
/// @code
/// auto moveListener = [](const Event<PlayerMoveData>& e) noexcept {
///     std::cout << "Player " << e.data.playerId
///               << " moved to (" << e.data.x << ", " << e.data.y << ")\n";
/// };
/// @endcode
///
/// 3. Bind Listeners to Event IDs
/// @code
/// // Bind to single event
/// EventHandler::instance().bindEventListener("player-move", moveListener);
///
/// // Bind to multiple events
/// EventHandler::instance().bindEventListener({"player-move", "player-teleport"}, moveListener);
/// @endcode
///
/// 4. Push Events to the Queue
/// @code
/// PlayerMoveData data{100.0f, 50.0f, 42};
/// EventHandler::instance().push("player-move", data);
/// @endcode
///
/// 5. Dispatch All Queued Events
/// @code
/// // Called once per frame in engine main loop
/// EventHandler::instance().dispatchEvents();
/// @endcode
///
/// @note
/// - Listeners must be noexcept for guaranteed exception safety
/// - Event data should be small as it's copied into the arena
/// - The system automatically handles object alignment and destruction
///
/// @warning Event listeners that throw exceptions will terminate the program via std::terminate

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
#include "Types.hpp"

namespace R3 {

/// @brief EventBase class is used as a base class for the event queue
struct EventBase {
protected:
    constexpr EventBase(hash::uuid id) noexcept
        : id{id} {}

public:
    const hash::uuid id; /// unique event id
};

/// @brief Derived Event which is templated for custom event data
/// @tparam Data Data stored in the event, this data will take up space on the queue so is should be small
template <typename Data>
struct Event : public EventBase {
    using DataType = Data;

    constexpr Event(hash::uuid id, const Data& data) noexcept
        : EventBase{id},
          data{data} {}

    constexpr Event(hash::uuid id, Data&& data) noexcept
        : EventBase{id},
          data{std::move(data)} {}

    template <typename... Args>
    constexpr Event(hash::uuid id, Args&&... args) noexcept
        : EventBase{id},
          data{args...} {}

    const Data data; /// event data
};

template <typename F>
using EventTypeDeduced = typename std::remove_reference_t<typename FunctionTraits<F>::template ArgType<0>>;

template <typename F, typename... Args>
concept EventListener = std::is_const_v<EventTypeDeduced<F>> and requires {
    { std::is_base_of_v<EventBase, EventTypeDeduced<F>> };
    { std::declval<F>()(std::declval<EventTypeDeduced<F>>()) } noexcept;
};

/// @brief Singleton event handler which you can push event to and bind listeners to
///
/// @code
/// EventHandler::instance().bindEventListener("key-press", [](const Event<KeyboardEventData>& e){
///     LOG_INFO("key pressed: {}", (int)e.data.key);
/// });
/// @endcode
class EventHandler {
private:
    using EventCallback = std::function<void(const EventBase&)>;

private:
    EventHandler() {
        m_eventQueue.reserve(10240); // 10kB
        m_eventArena.reserve(65536); // 64kB
    }

    ~EventHandler() noexcept { dispatchEvents(); };

    EventHandler(const EventHandler&)            = delete;
    EventHandler& operator=(const EventHandler&) = delete;

    EventHandler(EventHandler&&) noexcept            = delete;
    EventHandler& operator=(EventHandler&&) noexcept = delete;

public:
    /// @brief Single EventHandler instance
    /// @return global static instance of EventHandler
    static EventHandler& instance() {
        static EventHandler instance;
        return instance;
    }

    /// @brief Push an event onto the event queue
    /// @tparam Data Event DataType
    /// @param id    Event id e.g. "key-press"
    /// @param data  Copied event data payload
    template <typename Data>
    void push(hash::uuid id, const Data& data) {
        void* alignedPtr = allocateAligned<Event<Data>>();
        new (alignedPtr) Event<Data>{id, data}; // construct event in place
    }

    /// @brief Push an event onto the event queue
    /// @tparam Data Event DataType
    /// @param id    Event id e.g. "key-press"
    /// @param data  Moved event data payload
    template <typename Data>
    void push(hash::uuid id, Data&& data) {
        void* alignedPtr = allocateAligned<Event<Data>>();
        new (alignedPtr) Event<Data>{id, std::move(data)}; // construct event in place
    }

    /// @brief Emplace an event onto the event queue
    /// @tparam Data Event DataType
    /// @param id    Event id e.g. "key-press"
    /// @param args  Event data arguments to construct the event in place
    /// @note Must explicitly specify the Data type, this is useful for events that have no data
    template <typename Data, typename... Args>
    void emplace(hash::uuid id, Args&&... args) {
        void* alignedPtr = allocateAligned<Event<Data>>();
        new (alignedPtr) Event<Data>{id, std::forward<Args>(args)...}; // construct event in place
    }

    /// @brief Dispatch all event by calling every listener and then destructing the event objects
    void dispatchEvents() {
        // iterate queued event offsets
        for (auto&& [offset, deleter] : m_eventQueue) {
            // interpret event from bytes
            EventBase* event = reinterpret_cast<EventBase*>(&m_eventArena[offset]);
            // make callback calls
            auto range = m_eventRegistry.equal_range(event->id);
            for (auto& it = range.first; it != range.second; ++it) {
                it->second(*event);
            }
            // manually destructor because EventHandler owns the lifetime
            if (deleter != nullptr) {
                deleter(event);
            }
        }
        // all events are handled and all destructors called
        // can now safely overwrite memory
        m_eventQueue.clear();
        m_eventArena.clear();
    }

    /// @brief Bind an event listener to listen for events that have the same id
    /// @tparam F       Functor
    /// @param id       event id to listen to
    /// @param callback event callback triggered when id is emitted
    template <typename F>
    requires EventListener<F>
    void bindEventListener(hash::uuid id, F callback) {
        using EventType       = EventTypeDeduced<F>;
        EventCallback wrapper = [callback](const EventBase& base) { callback(static_cast<const EventType&>(base)); };
        m_eventRegistry.insert(std::make_pair(id, wrapper));
    }

    /// @brief Bind an event listener to listen for events that have the same id
    /// @tparam F       Functor
    /// @param id       event ids to listen to
    /// @param callback event callback triggered when id is emitted
    template <typename F>
    requires EventListener<F>
    void bindEventListener(std::initializer_list<hash::uuid> ids, F callback) {
        for (auto& id : ids) {
            bindEventListener(id, callback);
        }
    }

private:
    template <typename T>
    void* allocateAligned() {
        // calculate the next aligned offset
        const usize alignedOffset = getAlignedOffset<T>(m_eventArena.size());
        // resize arena to accommodate the aligned object
        m_eventArena.resize(alignedOffset + sizeof(T));
        // get properly aligned pointer
        void* alignedPtr = &m_eventArena[alignedOffset];
        // push the event byte offset to queue
        if constexpr (std::is_trivially_destructible_v<T>) {
            m_eventQueue.emplace_back(alignedOffset, nullptr);
        } else {
            m_eventQueue.emplace_back(alignedOffset, [](void* p) { ((T*)p)->~T(); });
        }
        return alignedPtr;
    }

    // cpp object alignment helper
    template <typename T>
    constexpr usize getAlignedOffset(usize curr) {
        return (curr + alignof(T) - 1) & ~(alignof(T) - 1);
    }

private:
    std::vector<std::pair<usize, void (*)(void*)>> m_eventQueue;    // tracks event indices and event destructors
    std::vector<std::byte> m_eventArena;                            // memory pool for allocations when pushing events
    std::unordered_multimap<uint64, EventCallback> m_eventRegistry; // mapping id to callback
};

} // namespace R3

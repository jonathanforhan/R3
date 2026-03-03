/// @file EventHandler.hpp
/// @brief Event handling system
///
/// The EventHandler provides a event dispatch system that uses compile-time type checking
/// to ensure event listeners are properly typed and exception-safe. Events are stored in a memory arena
/// for optimal performance and minimal heap fragmentation.
///
/// Basic Usage
///
/// 1. Define Custom Event Data
/// @code
/// struct R3_API PlayerMoveData {
///     float x, y;
///     int playerId;
/// };
/// @endcode
///
/// 2. Create Event Listeners
/// Event listeners must be noexcept lambdas that take a const DataType& parameter:
/// @code
/// auto moveListener = [](const PlayerMoveData& e) noexcept {
///     std::cout << "Player " << e.data.playerId
///               << " moved to (" << e.data.x << ", " << e.data.y << ")\n";
/// };
/// @endcode
///
/// 3. Bind Listeners to Event IDs
/// @code
/// // Bind to single event
/// EventHandler().bindEventListener("player-move", moveListener);
///
/// // Bind to multiple events
/// EventHandler().bindEventListener({"player-move", "player-teleport"}, moveListener);
/// @endcode
///
/// 4. Push Events to the Queue
/// @code
/// PlayerMoveData data{100.0f, 50.0f, 42};
/// EventHandler().push("player-move", data);
/// @endcode
///
/// 5. Dispatch All Queued Events
/// @code
/// // Called once per frame in engine main loop
/// EventHandler().dispatchEvents();
/// @endcode
///
/// @note
/// - Listeners must be noexcept for guaranteed exception safety
/// - Event data should be small as it's copied into the arena
/// - The system automatically handles object alignment and destruct R3_APIion
///
/// @warning Event listeners that throw exceptions will terminate the program via std::terminate

#pragma once

#include "engine/api/Api.hpp"

#include <concepts>
#include <cstddef>
#include <functional>
#include <initializer_list>
#include <iterator>
#include <list>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <vector>
#include "engine/api/Class.hpp"
#include "engine/api/FunctionTraits.hpp"
#include "engine/api/Hash.hpp"
#include "engine/api/TypeTraits.hpp"
#include "engine/api/Types.hpp"

namespace R3 {

/// @brief EventBase class R3_API is used as a base class R3_API for the event queue
struct R3_API EventBase {
protected:
    constexpr EventBase(hash::uuid id) noexcept
        : id{id} {}

public:
    const hash::uuid id; /// unique event id
};

/// @brief Derived Event which is templated for custom event data
/// @tparam Data Data stored in the event, this data will take up space on the queue so is should be small
template <typename Data>
struct R3_API Event : public EventBase {
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

/// @brief Specialization for void event data
template <>
struct R3_API Event<void> : public EventBase {
    using DataType = void;

    constexpr Event(hash::uuid id) noexcept
        : EventBase{id} {}
};

template <typename F>
concept VoidEventListener = requires {
    requires is_one_of_v<FunctionDeducedType<F>,
                         bool() noexcept,
                         bool() const noexcept,
                         void() noexcept,
                         void() const noexcept>;
};

template <typename F>
concept DataEventListener = requires {
    requires FunctionTraits<F>::Arity::value == 1;
    requires is_one_of_v<FunctionDeducedType<F>,
                         bool(FunctionDeducedParamType<F, 0>) noexcept,
                         bool(FunctionDeducedParamType<F, 0>) const noexcept,
                         void(FunctionDeducedParamType<F, 0>) noexcept,
                         void(FunctionDeducedParamType<F, 0>) const noexcept>;
};

template <typename F>
concept DataIdEventListener = requires {
    requires FunctionTraits<F>::Arity::value == 2;
    requires is_one_of_v<FunctionDeducedType<F>,
                         bool(FunctionDeducedParamType<F, 0>, hash::uuid) noexcept,
                         bool(FunctionDeducedParamType<F, 0>, hash::uuid) const noexcept,
                         bool(FunctionDeducedParamType<F, 0>, uint64) noexcept,
                         bool(FunctionDeducedParamType<F, 0>, uint64) const noexcept,
                         void(FunctionDeducedParamType<F, 0>, hash::uuid) noexcept,
                         void(FunctionDeducedParamType<F, 0>, hash::uuid) const noexcept,
                         void(FunctionDeducedParamType<F, 0>, uint64) noexcept,
                         void(FunctionDeducedParamType<F, 0>, uint64) const noexcept>;
};

template <typename F>
concept EventListener = VoidEventListener<F> || DataEventListener<F> || DataIdEventListener<F>;

/// @brief event handler which you can push event to and bind listeners to
///
/// @code
/// EventHandler().bindEventListener("key-press", [](const KeyboardEventData& e) noexcept {
///     LOG_INFO("key pressed: {}", (int)e.data.key);
/// });
/// @endcode
class R3_API EventHandler {
private:
    using EventCallback = std::function<bool(const EventBase&)>;

private:
    R3_COPY_DELETE(EventHandler);
    R3_MOVE_DELETE(EventHandler);

    EventHandler() {
        m_eventQueue.reserve(10240); // 10kB
        m_eventArena.reserve(65536); // 64kB
    }

    ~EventHandler() noexcept { dispatchEvents(); };

public:
    /// @brief Push an event onto the event queue
    /// @tparam Data Event DataType
    /// @param id    Event id e.g. "key-press"
    /// @param data  Copied event data payload
    template <typename Data>
    void push(hash::uuid id, const Data& data) {
        void* alignedPtr = allocateAligned<Event<Data>>();
        new (alignedPtr) Event<Data>{id, data}; // construct R3_API event in place
    }

    /// @brief Push an event onto the event queue
    /// @tparam Data Event DataType
    /// @param id    Event id e.g. "key-press"
    /// @param data  Moved event data payload
    template <typename Data>
    void push(hash::uuid id, Data&& data) {
        void* alignedPtr = allocateAligned<Event<Data>>();
        new (alignedPtr) Event<Data>{id, std::move(data)}; // construct R3_API event in place
    }

    /// @brief Emplace an event onto the event queue
    /// @tparam Data Event DataType
    /// @param id    Event id e.g. "key-press"
    /// @param args  Event data arguments to construct R3_API the event in place
    /// @note Must explicitly specify the Data type, this is useful for events that have no data
    template <typename Data = void, typename... Args>
    void emplace(hash::uuid id, Args&&... args) {
        void* alignedPtr = allocateAligned<Event<Data>>();
        new (alignedPtr) Event<Data>{id, std::forward<Args>(args)...}; // construct R3_API event in place
    }

    /// @brief Dispatch all event by calling every listener and then destruct R3_APIing the event objects
    void dispatchEvents() {
        // swap queues and arenas to allow pushing events while dispatching
        std::swap(m_dispatchQueue, m_eventQueue);
        std::swap(m_dispatchArena, m_eventArena);
        m_eventQueue.clear();
        m_eventArena.clear();
        // merge any listeners bound since last dispatch into the permanent registry
        m_dispatchRegistry.merge(m_eventRegistry);

        // iterate queued event offsets
        for (auto&& [offset, deleter] : m_dispatchQueue) {
            // interpret event from bytes
            EventBase* event = reinterpret_cast<EventBase*>(&m_dispatchArena[offset]);
            // make callback calls
            auto range = m_dispatchRegistry.equal_range(event->id);
            for (auto& it = range.first; it != range.second;) {
                bool removeListener = it->second(*event);
                // remove if listener returned true
                it = removeListener ? m_dispatchRegistry.erase(it) : std::next(it);
            }
            // manually destructor because EventHandler owns the lifetime
            if (deleter != nullptr) {
                deleter(event);
            }
        }
    }

    /// @brief Bind a void event listener to listen for events that have the same id
    /// @tparam F       Functor
    /// @param id       event id to listen to
    /// @param callback event callback triggered when id is emitted
    template <typename F>
    requires VoidEventListener<F>
    void bindEventListener(hash::uuid id, F callback) {
        using ResultType = FunctionDeducedResultType<F>;

        EventCallback wrapper = [callback](const EventBase&) mutable noexcept -> bool {
            if constexpr (std::is_same_v<ResultType, bool>) {
                return callback();
            } else if constexpr (std::is_same_v<ResultType, void>) {
                callback();
                return false;
            }
        };
        m_eventRegistry.insert(std::make_pair(id, wrapper));
    }

    /// @brief Bind a data event listener to listen for events that have the same id
    /// @tparam F       Functor
    /// @param id       event id to listen to
    /// @param callback event callback triggered when id is emitted
    template <typename F>
    requires DataEventListener<F>
    void bindEventListener(hash::uuid id, F callback) {
        using ResultType = FunctionDeducedResultType<F>;
        using ParamType0 = std::remove_reference_t<FunctionDeducedParamType<F, 0>>;

        EventCallback wrapper = [callback](const EventBase& base) mutable noexcept -> bool {
            const auto& event = static_cast<const Event<ParamType0>&>(base);

            if constexpr (std::is_same_v<ResultType, bool>) {
                return callback(event.data);
            } else if constexpr (std::is_same_v<ResultType, void>) {
                callback(event.data);
                return false;
            }
        };
        m_eventRegistry.insert(std::make_pair(id, wrapper));
    }

    /// @brief Bind a data id event listener to listen for events that have the same id
    /// @tparam F       Functor
    /// @param id       event id to listen to
    /// @param callback event callback triggered when id is emitted
    template <typename F>
    requires DataIdEventListener<F>
    void bindEventListener(hash::uuid id, F callback) {
        using ResultType = FunctionDeducedResultType<F>;
        using ParamType0 = std::remove_reference_t<FunctionDeducedParamType<F, 0>>;

        EventCallback wrapper = [callback](const EventBase& base) mutable noexcept -> bool {
            const auto& event = static_cast<const Event<ParamType0>&>(base);

            if constexpr (std::is_same_v<ResultType, bool>) {
                return callback(event.data, event.id);
            } else if constexpr (std::is_same_v<ResultType, void>) {
                callback(event.data, event.id);
                return false;
            }
        };
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

    // used dispatching to prevent modifying the queue while iterating
    decltype(m_eventQueue) m_dispatchQueue;
    decltype(m_eventArena) m_dispatchArena;
    decltype(m_eventRegistry) m_dispatchRegistry;

private:
    friend class R3_API Engine;
};

} // namespace R3

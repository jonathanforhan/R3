#pragma once

#include <functional>
#include "api/Function.hpp"
#include "api/Hash.hpp"
#include "api/Types.hpp"

/// @def EVENT hashes _Signal and binds _Payload to a new Event
#define EVENT(_Signal, _Payload) Event<::R3::HASH32(_Signal), _Payload>

namespace R3 {

/// @brief Type useing for the Scene event registry
using EventCallback = std::function<void(const void*)>;

/// @brief Deduces the type of the event from an event callback during binding
/// @tparam F lambda
template <typename F>
using EventTypeDeduced = typename std::remove_reference_t<typename FunctionTraits<F>::template ArgType<0>>;

/// @brief Event Listener constraint
/// used for binding an event callback to scene
/// @code
/// // example
/// Scene::bindEventListener([this](const MouseButtonPressEvent& e) {
///     if (e.payload.button == MouseButton::Left)
///         m_mouseDown = true;
/// });
/// @endcode
template <typename F>
concept EventListener = std::is_const_v<EventTypeDeduced<F>> and requires {
    typename std::enable_if_t<FunctionTraits<F>::Arity::value == 1>;       // lambda has 1 param
    typename std::enable_if_t<offsetof(EventTypeDeduced<F>, signal) == 0>; // event can be cast to uuid
    std::is_object_v<typename EventTypeDeduced<F>::PayloadType>;           // event has payload
};

/// @brief Event type used for the `` Scene Event Queue ``
/// Using the EVENT(_Signal, _Payload) macro for event creatation
/// @code
/// auto event = EVENT("on-foo-bar", SomePayload);
/// @endcode
/// @tparam Payload A trivially destructible struct that contains you Event Payload data
/// @tparam Signal A Hash of what Event Signal you are binding to
template <uuid32 Signal, typename Payload>
requires std::is_trivially_destructible_v<Payload>
class Event final {
public:
    using PayloadType = Payload;
    using SingalType = std::integral_constant<uuid32, Signal>;

    /// @brief Construct Event with given payload
    /// @note Recommended to use EVENT(_Signal, _Payload) macro
    /// @param payload
    Event(PayloadType payload)
        : payload(payload) {}

public:
    const uuid32 signal = Signal; ///< @brief Signal, a const uuid32 that we cast to get event type
    const PayloadType payload;    ///< @brief Payload, contains Event data
};

} // namespace R3

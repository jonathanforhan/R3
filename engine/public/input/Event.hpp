#pragma once

#include "api/Function.hpp"
#include "api/Hash.hpp"
#include "api/Types.hpp"

#define EVENT(_Signal, _Payload) Event<::R3::HASH32(_Signal), _Payload>

namespace R3 {

template <typename F>
using EventTypeDeduced = typename std::remove_reference_t<typename FunctionTraits<F>::template ArgType<0>>;

template <typename F>
concept EventListener = requires {
    typename std::enable_if_t<offsetof(EventTypeDeduced<F>, signal) == 0>;
    std::is_object_v<typename EventTypeDeduced<F>::PayloadType>;
    std::is_invocable_r_v<void, F&&, EventTypeDeduced<F>>;
};

template <uuid32 Signal, typename Payload>
requires requires { std::is_trivially_destructible_v<Payload>; }
class Event {
public:
    using PayloadType = Payload;
    using SingalType = std::integral_constant<uuid32, Signal>;

    Event(PayloadType payload)
        : payload(payload) {}

public:
    const uuid32 signal = Signal;
    const PayloadType payload;
};

} // namespace R3

#pragma once
#include "api/Hash.hpp"
#include "api/Log.hpp"
#include "api/Types.hpp"

#define EVENT(_Signal, _Payload) Event<HASH32(_Signal), _Payload>

namespace R3 {

template <typename F>
concept EventListener = requires { std::is_invocable_r_v<void, F&&, typename FunctionTraits<F>::template ArgType<0>>; };

template <uuid32 Signal, typename Payload>
requires requires { std::is_trivially_destructible_v<Payload>; }
class Event {
public:
    using PayloadType = Payload;

    Event(PayloadType payload)
        : payload(payload) {}

public:
    static constexpr uuid32 type = Signal; // this is for comp-time template query
    const uuid32 signal = Signal;          // this is for runtime, so we can cast to (uint32*) and get type
    const PayloadType payload;
};

} // namespace R3

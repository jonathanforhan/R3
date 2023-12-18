#pragma once
#include "api/Hash.hpp"
#include "api/Log.hpp"
#include "api/Types.hpp"

#define EVENT(_Signal, _Payload) Event<HASH32(_Signal), _Payload>

namespace R3 {

template <uuid32 Signal, typename Payload_T>
class Event {
public:
    using PayloadType = Payload_T;
    static_assert(std::is_trivially_destructible_v<PayloadType>, "payload cannot have a destructor, must be pure data");

    Event(PayloadType payload)
        : payload(payload) {}

public:
    static constexpr uuid32 type = Signal; // this is for comp-time template query
    const uuid32 signal = Signal;          // this is for runtime, so we can cast to (uint32*) and get type
    const PayloadType payload;
};

} // namespace R3

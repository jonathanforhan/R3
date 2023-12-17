#pragma once
#include "api/Types.hpp"
#include "api/Hash.hpp"

namespace R3 {

template <uuid Type>
class Event {
public:
    const uuid type = Type;
};

} // namespace R3

#pragma once

#include "api/Types.hpp"
#include "input/KeyboardEvent.hpp"
#include "systems/System.hpp"

namespace R3 {

class InputSystem : public System {
public:
    InputSystem();

    void tick(double);
};

} // namespace R3
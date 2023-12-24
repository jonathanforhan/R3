#pragma once

/// @file InputSystem.hpp

#include "api/Types.hpp"
#include "systems/System.hpp"

namespace R3 {

/// @brief System for User Input including Keyboard and Mouse Events
class InputSystem : public System {
public:
    /// @brief Create New System, binds Input callbacks
    InputSystem();

    void tick(double); ///< R3 tick
};

} // namespace R3
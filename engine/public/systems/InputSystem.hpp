#pragma once

/// @file InputSystem.hpp

#include <R3>
#include "systems/System.hpp"

namespace R3 {

/// @brief System for User Input including Keyboard and Mouse Events
class R3_API InputSystem : public System {
public:
    /// @brief Create New System, binds Input callbacks
    InputSystem();

    void tick(double); ///< R3 tick
};

} // namespace R3
#pragma once

/// @file ModelSystem.hpp

#include <R3>
#include "systems/System.hpp"

namespace R3 {

/// @brief Runs tick on all Models
class R3_API ModelSystem : public System {
public:
    /// @brief Standard R3 tick method
    /// @param dt delta time
    void tick(double dt) override;
};

} // namespace R3
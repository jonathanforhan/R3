#pragma once
#include "systems/System.hpp"

namespace R3 {

/// @brief Runs tick on all Models
class ModelSystem : public System {
public:
    /// @brief Standard R3 tick method
    /// @param delta time
    void tick(double) override;
};

} // namespace R3
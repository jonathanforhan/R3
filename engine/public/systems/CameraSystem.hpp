#pragma once

/// @file CameraSystem.hpp

#include <R3>
#include "systems/System.hpp"

namespace R3 {

/// @brief System for manipulating Camera (View and Projection)
class R3_API CameraSystem : public System {
public:
    /// @brief Create New System, binds event callbacks for Camera response
    CameraSystem();

    void tick(double dt) override; ///< R3 tick
};

} // namespace R3
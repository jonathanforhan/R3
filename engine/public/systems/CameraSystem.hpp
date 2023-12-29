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

private:
    struct ActiveKeys {
        bool w = false;
        bool a = false;
        bool s = false;
        bool d = false;
        bool e = false;
        bool q = false;
    } m_activeKeys;
    bool m_mouseDown = false;

    vec2 m_cursorPosition = vec2(0.0f);
    vec2 m_prevCursorPosition = vec2(0.0f);
};

} // namespace R3
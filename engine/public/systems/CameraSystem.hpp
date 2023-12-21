#pragma once
#include "InputSystem.hpp"

namespace R3 {

class CameraSystem : public InputSystem {
public:
    CameraSystem();

    void tick(double dt) override;

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
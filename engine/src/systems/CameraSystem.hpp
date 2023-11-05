#pragma once
#include "InputSystem.hpp"

namespace R3 {

class CameraSystem : public InputSystem {
public:
    CameraSystem();

    void tick(double dt) override;

private:
    float m_deltaTime{0.0f};
    bool m_mouseDown{false};
    dvec2 m_prevCursorPosition{dvec2(0.0, 0.0)};
};

} // namespace R3
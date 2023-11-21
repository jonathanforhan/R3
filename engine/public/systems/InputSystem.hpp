#pragma once
#include <array>
#include <functional>
#include "System.hpp"
#include "api/Types.hpp"

namespace R3 {

class InputSystem : public System {
public:
    explicit InputSystem();

    void setKeyBinding(Key key, std::function<void(InputAction)> keyBinding);
    void setMouseBinding(MouseButton button, std::function<void(InputAction)> mouseBinding);

    std::tuple<double, double> cursorPosition() const;

    void tick(double);

private:
    std::array<std::function<void(InputAction)>, 348> m_keyBindings;
    std::array<std::function<void(InputAction)>, 3> m_mouseBindings;
};

inline void InputSystem::setKeyBinding(Key key, std::function<void(InputAction)> keyBinding) {
    m_keyBindings[static_cast<usize>(key)] = keyBinding;
}

inline void InputSystem::setMouseBinding(MouseButton button, std::function<void(InputAction)> mouseBinding) {
    m_mouseBindings[static_cast<usize>(button)] = mouseBinding;
}

} // namespace R3
#pragma once

#include "api/Types.hpp"
#include "input/Event.hpp"
#include "input/InputCodes.hpp"

namespace R3 {

struct MouseButtonEventPayload {
    MouseButton button;
    InputModifier::Mask modifiers;
};

struct MouseCursorEventPayload {
    vec2 cursorPosition;
};

using MouseButtonPressEvent = EVENT("on-mouse-button-press", MouseButtonEventPayload);

using MouseButtonReleaseEvent = EVENT("on-mouse-button-release", MouseButtonEventPayload);

using MouseCursorEvent = EVENT("on-mouse-cursor-change", MouseCursorEventPayload);

} // namespace R3
#pragma once

/// @file MouseEvent.hpp
/// @brief Provides Mouse Event Types for Events and Listeners

#include "api/Types.hpp"
#include "input/Event.hpp"
#include "input/InputCodes.hpp"

namespace R3 {

/// @brief Mouse Button Event Payload
struct MouseButtonEventPayload {
    MouseButton button;            ///< Mouse Button activated
    InputModifier::Mask modifiers; // 8 bit mask of modifiers applied
};

/// @brief Mouse Cursor Event Payload
struct MouseCursorEventPayload {
    vec2 cursorPosition; ///< x by y floating point cursor position **normalized**
};

/// @brief Mouse Button Press Event "on-mouse-button-press"
using MouseButtonPressEvent = EVENT("on-mouse-button-press", MouseButtonEventPayload);

/// @brief Mouse Button Release Event "on-mouse-button-release"
using MouseButtonReleaseEvent = EVENT("on-mouse-button-release", MouseButtonEventPayload);

/// @brief Mouse Cursor Event "on-mouse-cursor-change"
using MouseCursorEvent = EVENT("on-mouse-cursor-change", MouseCursorEventPayload);

} // namespace R3

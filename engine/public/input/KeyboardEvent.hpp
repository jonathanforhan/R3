#pragma once

/// @file KeyboardEvent.hpp
/// @brief Provides KeyboardEvent types for Events and Listeners

#include "api/Types.hpp"
#include "input/Event.hpp"
#include "input/InputCodes.hpp"

namespace R3 {

/// @brief Keyboard Event Payload
struct KeyboardEventPayload {
    Key key;                       ///< Key code activated
    InputModifier::Mask modifiers; ///< 8 bit Mask of modifiers applied
};

/// @brief Key Press Event "on-key-press"
using KeyPressEvent = EVENT("on-key-press", KeyboardEventPayload);

/// @brief Key Repeat Event "on-key-repeat"
using KeyRepeatEvent = EVENT("on-key-repeat", KeyboardEventPayload);

/// @brief Key Release Event "on-key-release"
using KeyReleaseEvent = EVENT("on-key-release", KeyboardEventPayload);

} // namespace R3
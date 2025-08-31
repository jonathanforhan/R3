#pragma once

#include "engine/api/Api.hpp"
#include "engine/api/Types.hpp"
#include "engine/input/InputCodes.hpp"

namespace R3 {

/**
 * events:
 *   "key-press"
 *   "key-repeat"
 *   "key-release"
 */
struct R3_API KeyboardEvent {
    Key key;                  /**< Key code activated */
    InputModifiers modifiers; /**< 8 bit Mask of modifiers applied */
};

/**
 * events:
 *   - "mouse-press"
 *   - "mouse-release"
 */
struct R3_API MouseButtonEvent {
    MouseButton button;       /**< Mouse button activated */
    InputModifiers modifiers; /**< 8 bit Mask of modifiers applied */
};

/**
 * events:
 *   - "mouse-scroll"
 */
struct R3_API MouseScrollEvent {
    double xoffset; /**< Mouse scroll offset x */
    double yoffset; /**< Mouse scroll offset y */
};

/**
 * events:
 *   - "cursor-move"
 */
struct R3_API MouseCursorEvent {
    double xpos; /**< Mouse cursor position in x normalized */
    double ypos; /**< Mouse cursor position in y normalized */
};

} // namespace R3
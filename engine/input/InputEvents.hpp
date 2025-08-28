#pragma once

#include "InputCodes.hpp"
#include "api/Types.hpp"

namespace R3 {

/**
 * events:
 *   "key-press"
 *   "key-repeat"
 *   "key-release"
 */
struct KeyboardEvent {
    Key key;                  /**< Key code activated */
    InputModifiers modifiers; /**< 8 bit Mask of modifiers applied */
};

/**
 * events:
 *   - "mouse-press"
 *   - "mouse-release"
 */
struct MouseButtonEvent {
    MouseButton button;       /**< Mouse button activated */
    InputModifiers modifiers; /**< 8 bit Mask of modifiers applied */
};

/**
 * events:
 *   - "mouse-scroll"
 */
struct MouseScrollEvent {
    double xoffset; /**< Mouse scroll offset x */
    double yoffset; /**< Mouse scroll offset y */
};

/**
 * events:
 *   - "cursor-move"
 */
struct MouseCursorEvent {
    double xpos; /**< Mouse cursor position in x normalized */
    double ypos; /**< Mouse cursor position in y normalized */
};

} // namespace R3
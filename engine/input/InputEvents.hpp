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
    dvec2 offset; /**< Mouse scroll offset x and y */
};

/**
 * events:
 *   - "cursor-move"
 */
struct MouseCursorEvent {
    dvec2 cursorPosition; /**< Mouse cursor position in x and y normalized */
};

} // namespace R3
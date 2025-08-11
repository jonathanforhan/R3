#pragma once

#include "EventHandler.hpp"
#include "InputCodes.hpp"
#include "Types.hpp"

namespace R3 {

/**
 * events:
 *   "key-press"
 *   "key-repeat"
 *   "key-release"
 */
struct KeyboardEventData {
    Key key;                  /**< Key code activated */
    InputModifiers modifiers; /**< 8 bit Mask of modifiers applied */
};

/**
 * events:
 *   - "mouse-press"
 *   - "mouse-release"
 */
struct MouseButtonEventData {
    MouseButton button;       /**< Mouse button activated */
    InputModifiers modifiers; /**< 8 bit Mask of modifiers applied */
};

/**
 * events:
 *   - "cursor-move"
 */
struct MouseCursorEventData {
    fvec2 cursorPosition; /**< Mouse cursor position in x and y normalized */
};

} // namespace R3
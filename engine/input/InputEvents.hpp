#pragma once

#include "engine/api/Api.hpp"
#include "engine/api/Types.hpp"
#include "engine/core/Entity.hpp"
#include "engine/input/InputCodes.hpp"

namespace R3 {

namespace event {
static constexpr hash::uuid KeyPress   = "key-press";
static constexpr hash::uuid KeyRepeat  = "key-repeat";
static constexpr hash::uuid KeyRelease = "key-release";
} // namespace event
struct R3_API KeyboardEvent {
    Key key;                  /**< Key code activated */
    InputModifiers modifiers; /**< 8 bit Mask of modifiers applied */
};

namespace event {
static constexpr hash::uuid MousePress   = "mouse-press";
static constexpr hash::uuid MouseRelease = "mouse-release";
} // namespace event
struct R3_API MouseButtonEvent {
    MouseButton button;       /**< Mouse button activated */
    InputModifiers modifiers; /**< 8 bit Mask of modifiers applied */
    double xpos;              /**< Mouse cursor position in pixels, double to support subpixel precision */
    double ypos;              /**< Mouse cursor position in pixels, double to support subpixel precision */
};

namespace event {
static constexpr hash::uuid MouseScroll = "mouse-scroll";
} // namespace event
struct R3_API MouseScrollEvent {
    double xoffset; /**< Mouse scroll offset x */
    double yoffset; /**< Mouse scroll offset y */
};

namespace event {
static constexpr hash::uuid CursorMove = "cursor-move";
} // namespace event
struct R3_API MouseCursorEvent {
    double xpos; /**< Mouse cursor position in pixels, double to support subpixel precision */
    double ypos; /**< Mouse cursor position in pixels, double to support subpixel precision */
};

namespace event {
static constexpr hash::uuid HoveredEntity = "hovered-entity";
} // namespace event
struct R3_API HoveredEntityEvent {
    Entity entityID; /**< ID of the currently hovered entity, 0xFFFF'FFFF if no entity hovered */
};

} // namespace R3
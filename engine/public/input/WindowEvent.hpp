#pragma once

/// @file WindowEvent.hpp
/// @brief Provides Window Event Types for Events and Listeners

#include "api/Types.hpp"
#include "input/Event.hpp"

namespace R3 {

/// @brief Window Resize Payload
struct WindowResizePayload {
    int32 width;  ///< framebuffer width in pixels
    int32 height; ///< framebuffer height in pixels
};

/// @brief Window Resize Event "on-window-resize"
using WindowResizeEvent = EVENT("on-window-resize", WindowResizePayload);

/// @brief Empty Payload
struct WindowClosePayload {};

/// @brief Window Close Event "on-window-close"
using WindowCloseEvent = EVENT("on-window-close", WindowClosePayload);

} // namespace R3
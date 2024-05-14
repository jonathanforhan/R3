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

/// @brief Window Content Scale Payload
/// Contains the Content Scale of the window, useful for DPI adjustments
struct WindowContentScalePayload {
    float scaleX;
    float scaleY;
};

/// @brief Event that is triggered when the current Window's content scale changes
/// This can be because of a user settings change or a change in the current Monitor
using WindowContentScaleChangeEvent = EVENT("on-window-content-scale-change", WindowContentScalePayload);

} // namespace R3

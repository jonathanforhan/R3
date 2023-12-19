#pragma once

#include "api/Types.hpp"
#include "input/Event.hpp"

namespace R3 {

/// @brief Dimesions handled by swapchain due to extent limitations
struct WindowResizePayload {
    int32 width;
    int32 height;
};

using WindowResizeEvent = EVENT("on-window-resize", WindowResizePayload);

struct WindowClosePayload {};

using WindowCloseEvent = EVENT("on-window-close", WindowClosePayload);

} // namespace R3
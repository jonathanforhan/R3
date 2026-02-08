#pragma once

#include "engine/api/Api.hpp"
#include "engine/api/Types.hpp"

namespace R3 {

namespace event {
static constexpr hash::uuid WindowResize = "window-resize";
} // namespace event
struct R3_API WindowResizeEvent {
    int32 width;
    int32 height;
};

namespace event {
static constexpr hash::uuid WindowContentScale = "window-content-scale";
} // namespace event
struct R3_API WindowContentScaleEvent {
    float xscale;
    float yscale;
};

namespace event {
static constexpr hash::uuid WindowFocus = "window-focus";
} // namespace event
struct R3_API WindowFocusEvent {
    bool focused;
};

namespace event {
static constexpr hash::uuid WindowClose = "window-close";
} // namespace event
struct R3_API WindowCloseEvent{};

} // namespace R3

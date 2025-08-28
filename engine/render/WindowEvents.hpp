#pragma once

#include "api/Types.hpp"

namespace R3 {

struct WindowResizeEvent {
    int32 width;
    int32 height;
};

struct WindowContentScaleEvent {
    float xscale;
    float yscale;
};

struct WindowFocusEvent {
    bool focused;
};

struct WindowCloseEvent {};

} // namespace R3

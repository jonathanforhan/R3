#pragma once

#include "engine/api/Api.hpp"
#include "engine/api/Types.hpp"

namespace R3 {

struct R3_API WindowResizeEvent {
    int32 width;
    int32 height;
};

struct R3_API WindowContentScaleEvent {
    float xscale;
    float yscale;
};

struct R3_API WindowFocusEvent {
    bool focused;
};

struct R3_API WindowCloseEvent{};

} // namespace R3

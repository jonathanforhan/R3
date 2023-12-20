#pragma once

#include "api/Types.hpp"

namespace R3::vulkan {

struct PushConstant {
    alignas(16) mat4 view;
    alignas(16) mat4 projection;
};

} // namespace R3::vulkan
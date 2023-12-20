#pragma once

#include "api/Types.hpp"

namespace R3::vulkan {

struct UniformBufferObject {
    alignas(16) mat4 model;
};

} // namespace R3::vulkan
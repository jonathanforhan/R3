#pragma once

#include "api/Types.hpp"

namespace R3::vulkan {

// Layout of vulkan UBO aligned
struct UniformBufferObject {
    alignas(16) mat4 model;
};

} // namespace R3::vulkan
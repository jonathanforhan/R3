#pragma once

#include "api/Types.hpp"

namespace R3 {

struct UniformBufferObject {
    alignas(16) mat4 model;
    alignas(16) mat4 view;
    alignas(16) mat4 projection;
};

} // namespace R3
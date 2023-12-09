#pragma once

#include "api/Types.hpp"

namespace R3 {

struct UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 projection;
};

} // namespace R3
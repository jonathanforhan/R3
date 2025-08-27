#pragma once

#include "api/Types.hpp"

namespace R3 {

struct TransformComponent {
    alignas(16) fmat4 transform = fmat4(1.0f);
};

} // namespace R3

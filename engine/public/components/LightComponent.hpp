#pragma once
#include "api/Types.hpp"

namespace R3 {

struct LightComponent {
    vec3 position{0.0f};
    vec3 color{1.0f};
    float intensity{1.0f};
    float radius{1.0f};
};

} // namespace R3
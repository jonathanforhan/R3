#pragma once
#include "api/Types.hpp"

namespace R3 {

struct LightComponent {
    vec3 position;
    vec3 color;
    float intensity;
    float radius;
};

} // namespace R3
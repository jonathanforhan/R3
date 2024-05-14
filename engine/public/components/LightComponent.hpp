#pragma once

#include "api/Types.hpp"

namespace R3 {

struct R3_API LightComponent {
    vec3 position = vec3(0.0f);
    vec3 color = vec3(1.0f);
    float intensity = 1.0f;
};

} // namespace R3

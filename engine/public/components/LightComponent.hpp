#pragma once

/// @file LightComponent.hpp
// TODO

#include "api/Types.hpp"

namespace R3 {

struct R3_API LightComponent {
    vec3 position = 0.0f;
    vec3 color = 1.0f;
    float intensity = 1.0f;
    float radius = 1.0f;
};

} // namespace R3
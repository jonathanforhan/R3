#pragma once

#include "engine/api/Api.hpp"
#include "engine/api/Types.hpp"

namespace R3 {

struct R3_API LightComponent {
    fvec3 position  = fvec3(0.0f);
    fvec3 color     = fvec3(1.0f);
    float intensity = 1.0f;
};

} // namespace R3

#pragma once

#include "engine/api/Api.hpp"
#include "engine/api/Types.hpp"

namespace R3 {

enum class R3_API QueueType {
    Present,
    Graphics,
    Compute,
    Transfer,
};

enum class R3_API TextureType {
    Albedo            = 0,
    MetallicRoughness = 1,
    Normal            = 2,
    AmbientOcclusion  = 3,
    Emissive          = 4,
    CubeMap           = 5,
    ShadowCubeMap     = 6,
};

} // namespace R3

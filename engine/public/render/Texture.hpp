#pragma once

#include "api/Types.hpp"

namespace R3 {

struct PBRTextureResource {
    id albedo = ~usize(0);
    id metallicRoughness = ~usize(0);
    id normal = ~usize(0);
    id ambientOcclusion = ~usize(0);
    id emissive = ~usize(0);
};

enum class TextureType : uint8 {
    Nil = 0,
    Albedo = 1,
    MetallicRoughness = 2,
    Normal = 3,
    AmbientOcclusion = 4,
    Emissive = 5,
};

} // namespace R3
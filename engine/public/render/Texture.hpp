#pragma once

#include "api/Types.hpp"

namespace R3 {

struct R3_API PBRTextureResource {
    id albedo = ~id(0);
    id metallicRoughness = ~id(0);
    id normal = ~id(0);
    id ambientOcclusion = ~id(0);
    id emissive = ~id(0);
};

enum class R3_API TextureType : uint8 {
    Nil = 0,
    Albedo = 1,
    MetallicRoughness = 2,
    Normal = 3,
    AmbientOcclusion = 4,
    Emissive = 5,
};

} // namespace R3
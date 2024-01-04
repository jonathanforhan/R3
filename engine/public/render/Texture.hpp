#pragma once

#include <R3>

namespace R3 {

struct R3_API PBRTextureResource {
    id albedo = undefined;
    id metallicRoughness = undefined;
    id normal = undefined;
    id ambientOcclusion = undefined;
    id emissive = undefined;
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
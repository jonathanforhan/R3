#pragma once

#include <cstdint>
#include "api/Types.hpp"
#include "render/Flags.hpp"

namespace R3 {

struct MaterialComponent {
    usize albedoIndex            = SIZE_MAX;
    usize normalIndex            = SIZE_MAX;
    usize metallicRoughnessIndex = SIZE_MAX;
    usize ambientOcclusionIndex  = SIZE_MAX;
    usize emissiveIndex          = SIZE_MAX;

    void setTextureIndex(TextureType type, usize index) {
        switch (type) {
            case TextureType::Albedo:
                albedoIndex = index;
                break;
            case TextureType::Normal:
                normalIndex = index;
                break;
            case TextureType::MetallicRoughness:
                metallicRoughnessIndex = index;
                break;
            case TextureType::AmbientOcclusion:
                ambientOcclusionIndex = index;
                break;
            case TextureType::Emissive:
                emissiveIndex = index;
                break;
        }
    }
};

} // namespace R3

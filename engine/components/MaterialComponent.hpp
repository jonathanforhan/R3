#pragma once

#include <cstdint>
#include "engine/api/Api.hpp"
#include "engine/api/Class.hpp"
#include "engine/api/Types.hpp"
#include "engine/core/Engine.hpp"
#include "engine/core/Log.hpp"
#include "engine/core/ResourceManager.hpp"
#include "engine/render/Flags.hpp"
#include "engine/render/vulkan/vulkan-DescriptorSet.hpp"
#include "engine/render/vulkan/vulkan-GraphicsPipeline.hpp"
#include "engine/render/vulkan/vulkan-Texture.hpp"

namespace R3 {

struct R3_API MaterialComponent {
    R3_CTOR_DEFAULT(MaterialComponent);
    R3_COPY_DELETE(MaterialComponent);

    MaterialComponent(MaterialComponent&& other) noexcept {
        // this->* defaults to UINT32_MAX (don't put this in member intializer)
        iAlbedo            = std::exchange(other.iAlbedo, iAlbedo);
        iNormal            = std::exchange(other.iNormal, iNormal);
        iMetallicRoughness = std::exchange(other.iMetallicRoughness, iMetallicRoughness);
        iAmbientOcclusion  = std::exchange(other.iAmbientOcclusion, iAmbientOcclusion);
        iEmissive          = std::exchange(other.iEmissive, iEmissive);
    }

    MaterialComponent& operator=(MaterialComponent&& other) noexcept {
        if (this != &other) {
            iAlbedo            = std::exchange(other.iAlbedo, iAlbedo);
            iNormal            = std::exchange(other.iNormal, iNormal);
            iMetallicRoughness = std::exchange(other.iMetallicRoughness, iMetallicRoughness);
            iAmbientOcclusion  = std::exchange(other.iAmbientOcclusion, iAmbientOcclusion);
            iEmissive          = std::exchange(other.iEmissive, iEmissive);
        }
        return *this;
    }

    ~MaterialComponent() noexcept {
        GResourceManager()->unbindTexture(iAlbedo);
        GResourceManager()->unbindTexture(iNormal);
        GResourceManager()->unbindTexture(iMetallicRoughness);
        GResourceManager()->unbindTexture(iAmbientOcclusion);
        GResourceManager()->unbindTexture(iEmissive);
    }

    // Textures
    uint32 iAlbedo            = UINT32_MAX;
    uint32 iNormal            = UINT32_MAX;
    uint32 iMetallicRoughness = UINT32_MAX;
    uint32 iAmbientOcclusion  = UINT32_MAX;
    uint32 iEmissive          = UINT32_MAX;

    void setTextureSlot(TextureType type, uint32 slot) {
        switch (type) {
            case TextureType::Albedo:
                iAlbedo = slot;
                break;
            case TextureType::Normal:
                iNormal = slot;
                break;
            case TextureType::MetallicRoughness:
                iMetallicRoughness = slot;
                break;
            case TextureType::AmbientOcclusion:
                iAmbientOcclusion = slot;
                break;
            case TextureType::Emissive:
                iEmissive = slot;
                break;
        }
    }
};

} // namespace R3

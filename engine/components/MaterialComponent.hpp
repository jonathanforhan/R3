#pragma once

#include <cstdint>
#include "api/Types.hpp"
#include "core/ResourceManager.hpp"
#include "render/Flags.hpp"
#include "render/vulkan/vulkan-DescriptorSet.hpp"
#include "render/vulkan/vulkan-GraphicsPipeline.hpp"
#include "render/vulkan/vulkan-Texture.hpp"

namespace R3 {

struct MaterialComponent {
    // Textures
    Handle<vulkan::Texture> albedo;
    Handle<vulkan::Texture> normal;
    Handle<vulkan::Texture> metallicRoughness;
    Handle<vulkan::Texture> ambientOcclusion;
    Handle<vulkan::Texture> emissive;

    // Descriptor Sets
    std::vector<vulkan::DescriptorSet> descriptorSets;

    void setTextureHandle(TextureType type, Handle<vulkan::Texture> texture) {
        switch (type) {
            case TextureType::Albedo:
                albedo = std::move(texture);
                break;
            case TextureType::Normal:
                normal = std::move(texture);
                break;
            case TextureType::MetallicRoughness:
                metallicRoughness = std::move(texture);
                break;
            case TextureType::AmbientOcclusion:
                ambientOcclusion = std::move(texture);
                break;
            case TextureType::Emissive:
                emissive = std::move(texture);
                break;
        }
    }
};

} // namespace R3

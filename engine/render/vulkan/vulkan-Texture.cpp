#if R3_VULKAN

#include "vulkan-Texture.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <algorithm>
#include <cmath>
#include <vulkan/vulkan_core.h>
#include "Exception.hpp"
#include "Log.hpp"
#include "Types.hpp"
#include "vulkan-Buffer.hpp"
#include "vulkan-Image.hpp"
#include "vulkan-RenderContext.hpp"

namespace R3::vulkan {

static VkFormat getPreferredFormat(TextureType type) {
    switch (type) {
        case TextureType::Albedo:
            return VK_FORMAT_R8G8B8A8_SRGB; // sRGB for color data
        case TextureType::MetallicRoughness:
            return VK_FORMAT_R8G8B8A8_UNORM; // Linear for material properties
        case TextureType::Normal:
            return VK_FORMAT_R8G8B8A8_UNORM; // Linear for normals
        case TextureType::AmbientOcclusion:
            return VK_FORMAT_R8_UNORM; // Single channel is enough
        case TextureType::Emissive:
            return VK_FORMAT_R8G8B8A8_SRGB; // sRGB for emissive colors
        default:
            return VK_FORMAT_R8G8B8A8_UNORM; // default
    }
}

static bool supportsBlitting(VkPhysicalDevice physicalDevice, VkFormat format) {
    VkFormatProperties properties;
    vkGetPhysicalDeviceFormatProperties(physicalDevice, format, &properties);

    static constexpr VkFormatFeatureFlags requireFeatures =
        VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT;

    return (properties.optimalTilingFeatures & requireFeatures) == requireFeatures;
}

void Texture::create(RenderContext& ctx,
                     VkCommandBuffer cmd,
                     const uint8* raw,
                     usize width,
                     usize height,
                     TextureType type) {
    m_device = ctx.device();
    m_type   = type;

    if (raw == nullptr) {
        throw Exception{__FUNCTION__ " called with nullptr"};
    }

    const VkFormat preferredFormat = getPreferredFormat(type);

    if (!supportsBlitting(ctx.physicalDevice(), preferredFormat)) {
        LOG_WARNING("Texture does not support blitting");
    }

    const uint32 mipLevels = static_cast<uint32>(std::floor(std::log2(std::max(width, height)))) + 1;
    const usize imageSize  = width * height * 4;

    // create staging buffer for CPU writes
    Buffer stagingBuffer;
    stagingBuffer.allocate(ctx,
                           imageSize,
                           VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                           VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    stagingBuffer.copy(raw, imageSize);

    m_image.allocate(ctx,
                     preferredFormat,
                     VkExtent3D{(uint32)width, (uint32)height, 1},
                     mipLevels,
                     1,
                     VK_IMAGE_TILING_OPTIMAL,
                     VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
                     VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    //
}

void Texture::create(RenderContext& ctx, VkCommandBuffer cmd, const uint8* compressed, usize size, TextureType type) {
    int width, height, channels;
    uint8* raw = stbi_load_from_memory(compressed, static_cast<int>(size), &width, &height, &channels, 4);
    create(ctx, cmd, raw, width, height, type);
    stbi_image_free(raw);
}

void Texture::create(RenderContext& ctx, VkCommandBuffer cmd, const char* path, TextureType type) {
    int width, height, channels;
    uint8* raw = stbi_load(path, &width, &height, &channels, 4);
    create(ctx, cmd, raw, width, height, type);
    stbi_image_free(raw);
}

void Texture::destroy() noexcept {
    if (m_device != VK_NULL_HANDLE) {
        vkDestroySampler(m_device, m_sampler, nullptr);
        m_sampler = VK_NULL_HANDLE;
    }
    m_device = VK_NULL_HANDLE;

    m_image.free();
}

} // namespace R3::vulkan

#endif // R3_VULKAN
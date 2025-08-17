#if R3_VULKAN

#include "render/Texture.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <algorithm>
#include <cmath>
#include <vulkan/vulkan_core.h>
#include "Exception.hpp"
#include "Log.hpp"
#include "Types.hpp"
#include "render/Buffer.hpp"
#include "render/Flags.hpp"
#include "render/Image.hpp"
#include "render/RenderContext.hpp"

namespace R3 {

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
                           BufferUsageFlags::TransferSrc,
                           MemoryPropertyFlags::HostVisible | MemoryPropertyFlags::HostCoherent);
    stagingBuffer.copy(raw, imageSize);

    m_image.allocate(ctx,
                     preferredFormat,
                     uvec3(width, height, 1),
                     mipLevels,
                     1,
                     ImageTiling::Optimal,
                     ImageUsageFlags::TransferSrc | ImageUsageFlags::TransferDst | ImageUsageFlags::Sampled,
                     MemoryPropertyFlags::DeviceLocal);

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

void Texture::destroy() noexcept {}

} // namespace R3

#endif
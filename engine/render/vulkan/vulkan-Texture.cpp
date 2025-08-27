#include "vulkan-Texture.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <algorithm>
#include <cmath>
#include <filesystem>
#include <string>
#include <vulkan/vulkan_core.h>
#include "api/Exception.hpp"
#include "api/Types.hpp"
#include "core/Engine.hpp"
#include "core/Log.hpp"
#include "render/Flags.hpp"
#include "vulkan-Buffer.hpp"
#include "vulkan-Check.hpp"
#include "vulkan-CommandBuffer.hpp"
#include "vulkan-Handle.hpp"
#include "vulkan-Image.hpp"
#include "vulkan-RenderContext.hpp"

namespace R3::vulkan {

Texture::Texture(CommandBuffer& cmd,
                 const std::byte* raw,
                 usize width,
                 usize height,
                 TextureType type,
                 Buffer& stagingBuffer) {
    create(cmd, raw, width, height, type, stagingBuffer);
}

Texture::Texture(CommandBuffer& cmd, const std::byte* compressed, usize size, TextureType type, Buffer& stagingBuffer) {
    int width, height, channels;
    std::byte* raw = (std::byte*)stbi_load_from_memory(
        (const uint8*)compressed, static_cast<int>(size), &width, &height, &channels, 4);
    create(cmd, raw, width, height, type, stagingBuffer);
    stbi_image_free(raw);
}

Texture::Texture(CommandBuffer& cmd, const std::filesystem::path& filepath, TextureType type, Buffer& stagingBuffer) {
    std::string path = filepath.string();
    if (path.back() != '\0') {
        path.push_back('\0');
    }

    int width, height, channels;
    std::byte* raw = (std::byte*)stbi_load(path.c_str(), &width, &height, &channels, 4);
    create(cmd, raw, width, height, type, stagingBuffer);
    stbi_image_free(raw);
}

Texture::~Texture() noexcept {
    RenderContext& ctx = static_cast<RenderContext&>(Engine()->context());
    vkDestroySampler(ctx.device(), m_sampler, nullptr);
}

void Texture::create(CommandBuffer& cmd,
                     const std::byte* raw,
                     usize width,
                     usize height,
                     TextureType type,
                     Buffer& stagingBuffer) {
    RenderContext& ctx = static_cast<RenderContext&>(Engine()->context());

    try {
        const VkFormat preferredFormat = queryPreferredFormat(type);
        if (!supportsBlitting(preferredFormat)) {
            LOG_WARNING("Texture does not support blitting");
        }

        const VkExtent2D extent = {(uint32)width, (uint32)height};
        const uint32 mipLevels  = static_cast<uint32>(std::floor(std::log2(std::max(width, height)))) + 1;
        const usize imageSize   = width * height * 4;

        // create staging buffer for CPU writes
        stagingBuffer = Buffer{raw,
                               imageSize,
                               VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                               VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT};

        // image used for texture
        m_image = Image{
            preferredFormat,
            extent,
            mipLevels,
            VK_SAMPLE_COUNT_1_BIT,
            VK_IMAGE_TILING_OPTIMAL,
            VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
            VK_IMAGE_ASPECT_COLOR_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        };

        const VkImageMemoryBarrier2 barrier = {
            .sType               = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
            .pNext               = nullptr,
            .srcStageMask        = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
            .srcAccessMask       = VK_ACCESS_NONE,
            .dstStageMask        = VK_PIPELINE_STAGE_TRANSFER_BIT,
            .dstAccessMask       = VK_ACCESS_TRANSFER_WRITE_BIT,
            .oldLayout           = VK_IMAGE_LAYOUT_UNDEFINED,
            .newLayout           = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .image               = m_image.image(),
            .subresourceRange =
                {
                    .aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT,
                    .baseMipLevel   = 0,
                    .levelCount     = mipLevels,
                    .baseArrayLayer = 0,
                    .layerCount     = 1,
                },
        };
        cmd.transitionImageLayout(barrier);

        // copy staging buffer to image
        const VkBufferImageCopy bufferToImage = {
            .bufferOffset      = 0,
            .bufferRowLength   = 0,
            .bufferImageHeight = 0,
            .imageSubresource =
                {
                    .aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT,
                    .mipLevel       = 0,
                    .baseArrayLayer = 0,
                    .layerCount     = 1,
                },
            .imageOffset = {0, 0, 0},
            .imageExtent = {extent.width, extent.width, 1},
        };
        cmd.copyBufferToImage(
            stagingBuffer.buffer(), m_image.image(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, {&bufferToImage, 1});

        m_image.generateMipMaps(cmd, extent, mipLevels);

        VkPhysicalDeviceProperties properties;
        vkGetPhysicalDeviceProperties(ctx.physicalDevice(), &properties);

        // create sampler for texture
        const VkSamplerCreateInfo samplerInfo = {
            .sType                   = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
            .pNext                   = nullptr,
            .flags                   = {},
            .magFilter               = VK_FILTER_LINEAR,
            .minFilter               = VK_FILTER_LINEAR,
            .mipmapMode              = VK_SAMPLER_MIPMAP_MODE_LINEAR,
            .addressModeU            = VK_SAMPLER_ADDRESS_MODE_REPEAT,
            .addressModeV            = VK_SAMPLER_ADDRESS_MODE_REPEAT,
            .addressModeW            = VK_SAMPLER_ADDRESS_MODE_REPEAT,
            .mipLodBias              = 0.0f,
            .anisotropyEnable        = VK_TRUE,
            .maxAnisotropy           = properties.limits.maxSamplerAnisotropy,
            .compareEnable           = VK_FALSE,
            .compareOp               = VK_COMPARE_OP_ALWAYS,
            .minLod                  = 0.0f,
            .maxLod                  = static_cast<float>(mipLevels),
            .borderColor             = VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK,
            .unnormalizedCoordinates = VK_FALSE,
        };
        VK_CHECK(vkCreateSampler(ctx.device(), &samplerInfo, nullptr, &*m_sampler));
    } catch (const Exception& ex) {
        vkDestroySampler(ctx.device(), m_sampler, nullptr);
        throw ex;
    }
}

bool Texture::supportsBlitting(VkFormat format) noexcept {
    RenderContext& ctx = static_cast<RenderContext&>(Engine()->context());
    VkFormat fmt       = ctx.querySupportedFormat(
        {&format, 1},
        VK_IMAGE_TILING_OPTIMAL,
        VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT);
    return fmt != VK_FORMAT_UNDEFINED;
}

VkFormat Texture::queryPreferredFormat(TextureType type) const noexcept {
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

} // namespace R3::vulkan

#include "vulkan-Texture.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <filesystem>
#include <string>
#include <vector>
#include <volk.h>
#include <vulkan/vulkan_core.h>
#include "api/Assert.hpp"
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
    R3_ASSERT(raw && "Texture raw data is null");
    create(cmd, raw, width, height, type, stagingBuffer);
}

Texture::Texture(CommandBuffer& cmd, const std::byte* compressed, usize size, TextureType type, Buffer& stagingBuffer) {
    R3_ASSERT(compressed && "Texture raw data is null");
    int width, height, channels;
    std::byte* raw = (std::byte*)stbi_load_from_memory(
        (const uint8*)compressed, static_cast<int>(size), &width, &height, &channels, 4);
    create(cmd, raw, width, height, type, stagingBuffer);
    stbi_image_free(raw);
}

Texture::Texture(CommandBuffer& cmd, const std::filesystem::path& filepath, TextureType type, Buffer& stagingBuffer) {
    R3_ASSERT(std::filesystem::exists(filepath) && "Ensure valid filepath");
    int width, height, channels;
    std::byte* raw = (std::byte*)stbi_load(filepath.string().c_str(), &width, &height, &channels, 4);
    create(cmd, raw, width, height, type, stagingBuffer);
    stbi_image_free(raw);
}

Texture::Texture(CommandBuffer& cmd,
                 const std::array<std::filesystem::path, 6>& facePaths,
                 TextureType type,
                 Buffer& stagingBuffer) {
    std::array<const std::byte*, 6> faceData;
    std::array<int, 6> widths, heights, channels;

    // Load all 6 faces
    for (int i = 0; i < 6; ++i) {
        R3_ASSERT(std::filesystem::exists(facePaths[i]) && "Cube map face file does not exist");
        faceData[i] = (std::byte*)stbi_load(facePaths[i].string().c_str(), &widths[i], &heights[i], &channels[i], 4);
        R3_ASSERT(faceData[i] && "Failed to load cube map face");

        // Ensure all faces have the same dimensions
        if (i > 0) {
            R3_ASSERT(widths[i] == widths[0] && heights[i] == heights[0] &&
                      "All cube map faces must have the same dimensions");
        }
    }

    // Convert to const pointers for createFromFaces
    createCubeMap(cmd, faceData, widths[0], heights[0], type, stagingBuffer);

    // Free loaded image data
    for (int i = 0; i < 6; ++i) {
        stbi_image_free(const_cast<std::byte*>(faceData[i]));
    }
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
        stagingBuffer = Buffer{raw, imageSize, BufferPreset::Staging};

        // image used for texture
        m_image = Image{
            VkImageCreateInfo{
                .sType       = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
                .imageType   = VK_IMAGE_TYPE_2D,
                .format      = preferredFormat,
                .extent      = {extent.width, extent.height, 1},
                .mipLevels   = mipLevels,
                .arrayLayers = 1,
                .samples     = VK_SAMPLE_COUNT_1_BIT,
                .tiling      = VK_IMAGE_TILING_OPTIMAL,
                .usage = VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
            },
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
            .imageExtent = {extent.width, extent.height, 1},
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

void Texture::createCubeMap(CommandBuffer& cmd,
                            std::array<const std::byte*, 6> faces,
                            usize width,
                            usize height,
                            TextureType type,
                            Buffer& stagingBuffer) {
    RenderContext& ctx = static_cast<RenderContext&>(Engine()->context());

    const VkFormat format   = queryPreferredFormat(type);
    const VkExtent2D extent = {static_cast<uint32>(width), static_cast<uint32>(height)};
    const uint32 mipLevels  = static_cast<uint32>(std::floor(std::log2(std::max(width, height)))) + 1;
    const usize faceSize    = width * height * 4; // 4 bytes per pixel (RGBA)
    const usize totalSize   = faceSize * 6;

    // Create staging buffer for all 6 faces
    std::vector<std::byte> allFaceData(totalSize);
    for (usize i = 0; i < 6; ++i) {
        std::copy(faces[i], faces[i] + faceSize, allFaceData.begin() + i * faceSize);
    }

    stagingBuffer = Buffer{allFaceData.data(), totalSize, BufferPreset::Staging};

    // Create cube map image (note: 6 array layers for cube faces)
    m_image = Image{
        VkImageCreateInfo{
            .sType       = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
            .flags       = VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT,
            .imageType   = VK_IMAGE_TYPE_2D,
            .format      = format,
            .extent      = {extent.width, extent.height, 1},
            .mipLevels   = mipLevels,
            .arrayLayers = 6, // 6 faces for cube map
            .samples     = VK_SAMPLE_COUNT_1_BIT,
            .tiling      = VK_IMAGE_TILING_OPTIMAL,
            .usage = VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
        },
        VK_IMAGE_ASPECT_COLOR_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
    };

    // Transition image layout for transfer
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
                .layerCount     = 6 // All 6 faces
            },
    };
    cmd.transitionImageLayout(barrier);

    // Copy each face from staging buffer to image
    std::vector<VkBufferImageCopy> copyRegions;
    for (usize face = 0; face < 6; ++face) {
        VkBufferImageCopy copyRegion = {
            .bufferOffset      = face * faceSize,
            .bufferRowLength   = 0,
            .bufferImageHeight = 0,
            .imageSubresource =
                {
                    .aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT,
                    .mipLevel       = 0,
                    .baseArrayLayer = static_cast<uint32>(face),
                    .layerCount     = 1,
                },
            .imageOffset = {0, 0, 0},
            .imageExtent = {extent.width, extent.height, 1},
        };
        copyRegions.push_back(copyRegion);
    }

    cmd.copyBufferToImage(stagingBuffer.buffer(), m_image.image(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, copyRegions);

    // Generate mipmaps for cube map (if supported)
    if (supportsBlitting(format)) {
        m_image.generateMipMaps(cmd, extent, mipLevels, 6);
    }

    // Create sampler for cube map
    VkPhysicalDeviceProperties properties;
    vkGetPhysicalDeviceProperties(ctx.physicalDevice(), &properties);

    const VkSamplerCreateInfo samplerInfo = {
        .sType                   = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
        .pNext                   = nullptr,
        .flags                   = 0,
        .magFilter               = VK_FILTER_LINEAR,
        .minFilter               = VK_FILTER_LINEAR,
        .mipmapMode              = VK_SAMPLER_MIPMAP_MODE_LINEAR,
        .addressModeU            = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
        .addressModeV            = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
        .addressModeW            = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
        .mipLodBias              = 0.0f,
        .anisotropyEnable        = VK_TRUE,
        .maxAnisotropy           = properties.limits.maxSamplerAnisotropy,
        .compareEnable           = VK_FALSE,
        .compareOp               = VK_COMPARE_OP_ALWAYS,
        .minLod                  = 0.0f,
        .maxLod                  = static_cast<float>(mipLevels),
        .borderColor             = VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK,
        .unnormalizedCoordinates = VK_FALSE,
    };
    VK_CHECK(vkCreateSampler(ctx.device(), &samplerInfo, nullptr, &*m_sampler));
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
            return VK_FORMAT_R8G8B8A8_UNORM; // Single channel is enough
        case TextureType::Emissive:
            return VK_FORMAT_R8G8B8A8_SRGB; // sRGB for emissive colors
        case TextureType::CubeMap:
            return VK_FORMAT_R8G8B8A8_SRGB; // sRGB for environment maps
        default:
            return VK_FORMAT_R8G8B8A8_UNORM; // default
    }
}

} // namespace R3::vulkan

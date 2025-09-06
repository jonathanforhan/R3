#include "vulkan-Texture.hpp"

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstring>
#include <filesystem>
#include <memory>
#include <vulkan/vulkan.h>
#include "api/Assert.hpp"
#include "api/Exception.hpp"
#include "api/Types.hpp"
#include "core/Engine.hpp"
#include "core/Log.hpp"
#include "media/ImageLoader.hpp"
#include "render/Buffer.hpp"
#include "render/Flags.hpp"
#include "render/Image.hpp"
#include "vulkan-Check.hpp"
#include "vulkan-CommandBuffer.hpp"
#include "vulkan-Handle.hpp"
#include "vulkan-RenderContext.hpp"

namespace R3::vulkan {

Texture::Texture(CommandBuffer& cmd,
                 const std::byte* raw,
                 usize width,
                 usize height,
                 uint32 channels,
                 TextureType type,
                 Buffer& stagingBuffer) {
    create(cmd, raw, width, height, channels, type, stagingBuffer);
}

Texture::Texture(CommandBuffer& cmd, const std::filesystem::path& filepath, TextureType type, Buffer& stagingBuffer) {
    auto imgDesc = ImageLoader().loadImageFile(filepath);
    create(cmd, imgDesc.data.get(), imgDesc.width, imgDesc.height, imgDesc.channels, type, stagingBuffer);
}

Texture::~Texture() noexcept {
    RenderContext& ctx = GEngine()->RenderContext<RenderContext>();
    vkDestroySampler(ctx.device(), m_sampler, nullptr);
}

void Texture::create(CommandBuffer& cmd,
                     const std::byte* raw,
                     usize width,
                     usize height,
                     uint32 channels,
                     TextureType type,
                     Buffer& stagingBuffer) {
    R3_ASSERT(type != TextureType::CubeMap, "Use createCubeMap for CubeMap textures");

    RenderContext& ctx = GEngine()->RenderContext<RenderContext>();

    try {
        const VkFormat preferredFormat = queryPreferredFormat(type);
        if (!supportsBlitting(preferredFormat)) {
            LOG_WARNING("Texture format does not support blitting");
        }
        const uint32 preferredChannels = queryPreferredChannels(type);

        const usize3 extent    = {width, height, 1};
        const uint32 mipLevels = static_cast<uint32>(std::floor(std::log2(std::max(width, height)))) + 1;
        const usize imgSize    = width * height * preferredChannels;
        const usize rawSize    = width * height * channels;

        stagingBuffer = Buffer{imgSize, BufferUsage::HostStaging};

        // copy data to staging buffer with proper channel mapping
        switch (type) {
            case TextureType::MetallicRoughness: {
                R3_ASSERT(channels >= 3);
                // copy the GB channels from raw to staging buffer's RG channels
                for (usize rg = 0, ch = 0; rg < imgSize; rg += preferredChannels, ch += channels) {
                    R3_ASSERT(ch < rawSize);
                    stagingBuffer.copy(&raw[ch + 1], rg, 2);
                }
                break;
            }
            case TextureType::Normal: {
                R3_ASSERT(channels >= 3);
                // copy the RG channels from raw to staging buffer's RG channels
                for (usize rg = 0, ch = 0; rg < imgSize; rg += preferredChannels, ch += channels) {
                    R3_ASSERT(ch < rawSize);
                    stagingBuffer.copy(&raw[ch], rg, 2);
                }
                break;
            }
            case TextureType::AmbientOcclusion: {
                // copy the R channel from raw to staging buffer's R channel
                for (usize r = 0, ch = 0; r < imgSize; r += preferredChannels, ch += channels) {
                    R3_ASSERT(ch < rawSize);
                    stagingBuffer.copy(&raw[ch], r, 1);
                }
                break;
            }
            // case TextureType::Albedo:
            // case TextureType::Emissive:
            default: {
                R3_ASSERT(preferredChannels == 4);
                if (channels == 4) {
                    for (usize rgba = 0, ch = 0; rgba < imgSize; rgba += preferredChannels, ch += channels) {
                        R3_ASSERT(ch < rawSize);
                        stagingBuffer.copy(&raw[ch], rgba, channels);
                    }
                } else {
                    uint8 color[4] = {0, 0, 0, 255};
                    for (usize rgba = 0, ch = 0; rgba < imgSize; rgba += preferredChannels, ch += channels) {
                        R3_ASSERT(ch < rawSize);
                        std::memcpy(color, &raw[ch], channels);
                        stagingBuffer.copy(color, rgba, sizeof(color));
                    }
                }
                break;
            }
        }

        // image used for texture
        m_image = Image{extent, mipLevels, 1, ImageUsage::Texture, Format(preferredFormat)};

        const VkImageMemoryBarrier2 barrier = {
            .sType               = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
            .srcStageMask        = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
            .srcAccessMask       = VK_ACCESS_NONE,
            .dstStageMask        = VK_PIPELINE_STAGE_TRANSFER_BIT,
            .dstAccessMask       = VK_ACCESS_TRANSFER_WRITE_BIT,
            .oldLayout           = VK_IMAGE_LAYOUT_UNDEFINED,
            .newLayout           = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .image               = m_image.imageHandle(),
            .subresourceRange =
                {
                    .aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT,
                    .baseMipLevel   = 0,
                    .levelCount     = mipLevels,
                    .baseArrayLayer = 0,
                    .layerCount     = 1,
                },
        };

        cmd.pipelineBarrier({
            .sType                   = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
            .imageMemoryBarrierCount = 1,
            .pImageMemoryBarriers    = &barrier,
        });

        // copy staging buffer to image
        const VkBufferImageCopy2 bufferToImage = {
            .sType             = VK_STRUCTURE_TYPE_BUFFER_IMAGE_COPY_2,
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
            .imageExtent = {static_cast<uint32>(extent.x), static_cast<uint32>(extent.y), 1},
        };

        cmd.copyBufferToImage({
            .sType          = VK_STRUCTURE_TYPE_COPY_BUFFER_TO_IMAGE_INFO_2,
            .srcBuffer      = stagingBuffer.bufferHandle(),
            .dstImage       = m_image.imageHandle(),
            .dstImageLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            .regionCount    = 1,
            .pRegions       = &bufferToImage,
        });

        m_image.generateMipMaps(cmd);

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
            .maxLod                  = VK_LOD_CLAMP_NONE,
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
    RenderContext& ctx = GEngine()->RenderContext<RenderContext>();
    VkFormat fmt       = ctx.querySupportedFormat(
        {&format, 1},
        VK_IMAGE_TILING_OPTIMAL,
        VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT);
    return fmt != VK_FORMAT_UNDEFINED;
}

VkFormat Texture::queryPreferredFormat(TextureType type) const noexcept {
#if R3_TEXTURE_COMPRESSION /* TODO */
    switch (type) {
        case TextureType::Albedo:
            return VK_FORMAT_BC7_SRGB_BLOCK; // 4 channel
        case TextureType::MetallicRoughness:
            return VK_FORMAT_BC5_UNORM_BLOCK; // 2 channel
        case TextureType::Normal:
            return VK_FORMAT_BC5_UNORM_BLOCK; // 2 channel
        case TextureType::AmbientOcclusion:
            return VK_FORMAT_BC4_UNORM_BLOCK; // 1 channel
        case TextureType::Emissive:
            return VK_FORMAT_BC7_UNORM_BLOCK; // 4 channel
        case TextureType::CubeMap:
            return VK_FORMAT_BC7_SRGB_BLOCK; // 4 channel
        default:
            return VK_FORMAT_BC7_UNORM_BLOCK; // 4 channel
    }
#else
    switch (type) {
        case TextureType::Albedo:
            return VK_FORMAT_R8G8B8A8_SRGB; // 4 channel
        case TextureType::MetallicRoughness:
            return VK_FORMAT_R8G8_UNORM; // 2 channel
        case TextureType::Normal:
            return VK_FORMAT_R8G8_UNORM; // 2 channel
        case TextureType::AmbientOcclusion:
            return VK_FORMAT_R8_UNORM; // 1 channel
        case TextureType::Emissive:
            return VK_FORMAT_R8G8B8A8_UNORM; // 4 channel
        case TextureType::CubeMap:
            return VK_FORMAT_R8G8B8A8_SRGB; // 4 channel
        case TextureType::ShadowCubeMap:
            return VK_FORMAT_D32_SFLOAT;
        default:
            return VK_FORMAT_R8G8B8A8_UNORM; // 4 channel
    }
#endif
}

uint32 Texture::queryPreferredChannels(TextureType type) const noexcept {
    switch (type) {
        case TextureType::Albedo:
            return 4; // RGBA
        case TextureType::MetallicRoughness:
            return 2; // RGA (GB -> RG)
        case TextureType::Normal:
            return 2; // RG
        case TextureType::AmbientOcclusion:
            return 1; // R
        case TextureType::Emissive:
            return 4; // RGBA
        case TextureType::CubeMap:
            return 4; // RGBA
        case TextureType::ShadowCubeMap:
            return 4; // RGBA
        default:
            return 4; // default
    }
}

} // namespace R3::vulkan

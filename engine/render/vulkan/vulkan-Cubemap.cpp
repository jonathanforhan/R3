#include "vulkan-Cubemap.hpp"

#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <filesystem>
#include <memory>
#include <vulkan/vulkan.h>
#include "api/Assert.hpp"
#include "api/Exception.hpp"
#include "api/Types.hpp"
#include "core/Engine.hpp"
#include "media/ImageLoader.hpp"
#include "render/Buffer.hpp"
#include "render/Flags.hpp"
#include "render/Image.hpp"
#include "vulkan-Check.hpp"
#include "vulkan-CommandBuffer.hpp"
#include "vulkan-Handle.hpp"
#include "vulkan-RenderContext.hpp"

namespace R3::vulkan {

Cubemap::Cubemap(CommandBuffer& cmd,
                 const std::array<const std::byte*, 6>& raw,
                 usize width,
                 usize height,
                 uint32 channels,
                 TextureType type,
                 Buffer& stagingBuffer) {
    create(cmd, raw, width, height, channels, type, stagingBuffer);
}

Cubemap::Cubemap(CommandBuffer& cmd,
                 const std::array<std::filesystem::path, 6>& facePaths,
                 TextureType type,
                 Buffer& stagingBuffer) {
    std::array<ImageLoader::ImageDescriptor, 6> imgDescs;
    std::array<const std::byte*, 6> faceData{};

    for (int i = 0; i < 6; ++i) {
        imgDescs[i] = ImageLoader().loadImageFile(facePaths[i], 4);
        faceData[i] = imgDescs[i].data.get();
    }
    create(cmd, faceData, imgDescs[0].width, imgDescs[0].height, 4, type, stagingBuffer);
}

Cubemap::~Cubemap() noexcept {
    RenderContext& ctx = GEngine()->RenderContext<RenderContext>();
    vkDestroySampler(ctx.device(), m_sampler, nullptr);
}

void Cubemap::create(CommandBuffer& cmd,
                     std::array<const std::byte*, 6> faces,
                     usize width,
                     usize height,
                     uint32 channels,
                     TextureType type,
                     Buffer& stagingBuffer) {
    R3_ASSERT(type == TextureType::CubeMap || type == TextureType::ShadowCubeMap,
              "Cubemap type must be CubeMap for createCubeMap");
    R3_ASSERT(channels == 4);

    RenderContext& ctx = GEngine()->RenderContext<RenderContext>();

    try {
        const VkFormat format  = queryPreferredFormat(type);
        const usize3 extent    = {width, height, 1};
        const uint32 mipLevels = static_cast<uint32>(std::floor(std::log2(std::max(width, height)))) + 1;
        const usize faceSize   = width * height * channels;

        VkImageAspectFlags aspectMask =
            type == TextureType::ShadowCubeMap ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT;
        ImageUsageFlags usage =
            type == TextureType::ShadowCubeMap ? ImageUsage::DepthStencilAttachment : ImageUsage::Texture;

        // Create cube map image (note: 6 array layers for cube faces)
        m_image = Image{extent, mipLevels, 1, usage, Format(format), ImageType::ImageCube};

        // Create staging buffer for all 6 faces
        stagingBuffer = Buffer{faceSize * 6, BufferUsage::HostStaging};
        for (usize i = 0; i < 6; ++i) {
            if (faces[i] != nullptr) {
                stagingBuffer.copy(faces[i], i * faceSize, faceSize);
            } else {
                goto l_skip_copy;
            }
        }

        // Transition image layout for transfer
        VkImageMemoryBarrier2 barrier;
        barrier = {
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
            .image               = m_image.imageHandle(),
            .subresourceRange =
                {
                    .aspectMask     = aspectMask,
                    .baseMipLevel   = 0,
                    .levelCount     = mipLevels,
                    .baseArrayLayer = 0,
                    .layerCount     = 6 // All 6 faces
                },
        };

        cmd.pipelineBarrier({
            .sType                   = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
            .imageMemoryBarrierCount = 1,
            .pImageMemoryBarriers    = &barrier,
        });

        // Copy each face from staging buffer to image
        std::array<VkBufferImageCopy2, 6> copyRegions;
        for (usize face = 0; face < 6; ++face) {
            copyRegions[face] = {
                .sType             = VK_STRUCTURE_TYPE_BUFFER_IMAGE_COPY_2,
                .bufferOffset      = face * faceSize,
                .bufferRowLength   = 0,
                .bufferImageHeight = 0,
                .imageSubresource =
                    {
                        .aspectMask     = aspectMask,
                        .mipLevel       = 0,
                        .baseArrayLayer = static_cast<uint32>(face),
                        .layerCount     = 1,
                    },
                .imageOffset = {0, 0, 0},
                .imageExtent = {static_cast<uint32>(extent.x), static_cast<uint32>(extent.y), 1},
            };
        }

        cmd.copyBufferToImage({
            .sType          = VK_STRUCTURE_TYPE_COPY_BUFFER_TO_IMAGE_INFO_2,
            .srcBuffer      = stagingBuffer.bufferHandle(),
            .dstImage       = m_image.imageHandle(),
            .dstImageLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            .regionCount    = static_cast<uint32>(copyRegions.size()),
            .pRegions       = copyRegions.data(),
        });

        // Generate mipmaps for cube map (if supported)
        if (type != TextureType::ShadowCubeMap && supportsBlitting(format)) {
            m_image.generateMipMaps(cmd);
        }

#if 0
        // generate 6 individual image views for each face
        for (uint32 face = 0; face < 6; ++face) {
            VkImageViewCreateInfo viewInfo = {
                .sType    = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
                .image    = m_image.image(),
                .viewType = VK_IMAGE_VIEW_TYPE_2D,
                .format   = format,
                .subresourceRange =
                    {
                        .aspectMask     = VK_IMAGE_ASPECT_DEPTH_BIT,
                        .baseMipLevel   = 0,
                        .levelCount     = 1,
                        .baseArrayLayer = face,
                        .layerCount     = 1,
                    },
            };
            vkCreateImageView(ctx.device(), &viewInfo, nullptr, &*m_faceViews[face]);
        }
#endif

    l_skip_copy:

        // Create sampler for cube map
        VkPhysicalDeviceProperties properties;
        vkGetPhysicalDeviceProperties(ctx.physicalDevice(), &properties);

        VkFilter filter = type == TextureType::ShadowCubeMap ? VK_FILTER_NEAREST : VK_FILTER_LINEAR;
        VkSamplerAddressMode addressMode =
            type == TextureType::ShadowCubeMap ? VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE : VK_SAMPLER_ADDRESS_MODE_REPEAT;
        VkBool32 anisotropy   = type == TextureType::ShadowCubeMap ? VK_FALSE : VK_TRUE;
        VkCompareOp compareOp = type == TextureType::ShadowCubeMap ? VK_COMPARE_OP_LESS_OR_EQUAL : VK_COMPARE_OP_ALWAYS;
        VkBool32 compareEnable = type == TextureType::ShadowCubeMap ? VK_TRUE : VK_FALSE;

        const VkSamplerCreateInfo samplerInfo = {
            .sType                   = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
            .magFilter               = filter,
            .minFilter               = filter,
            .mipmapMode              = VK_SAMPLER_MIPMAP_MODE_LINEAR,
            .addressModeU            = addressMode,
            .addressModeV            = addressMode,
            .addressModeW            = addressMode,
            .mipLodBias              = 0.0f,
            .anisotropyEnable        = anisotropy,
            .maxAnisotropy           = properties.limits.maxSamplerAnisotropy,
            .compareEnable           = compareEnable,
            .compareOp               = compareOp,
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

bool Cubemap::supportsBlitting(VkFormat format) noexcept {
    RenderContext& ctx = GEngine()->RenderContext<RenderContext>();
    VkFormat fmt       = ctx.querySupportedFormat(
        {&format, 1},
        VK_IMAGE_TILING_OPTIMAL,
        VK_FORMAT_FEATURE_BLIT_SRC_BIT | VK_FORMAT_FEATURE_BLIT_DST_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT);
    return fmt != VK_FORMAT_UNDEFINED;
}

VkFormat Cubemap::queryPreferredFormat(TextureType type) const noexcept {
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

uint32 Cubemap::queryPreferredChannels(TextureType type) const noexcept {
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

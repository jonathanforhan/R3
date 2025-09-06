#include "engine/render/Image.hpp"

#include <vulkan/vulkan.h>
#include "api/MovableHandle.hpp"
#include "api/Types.hpp"
#include "core/Engine.hpp"
#include "render/Flags.hpp"
#include "vulkan-Check.hpp"
#include "vulkan-CommandBuffer.hpp"
#include "vulkan-RenderContext.hpp"

#define TO_VK_USAGE_FLAGS(usage)    ((VkImageUsageFlags)((usage) & 0x0000'FFFF) | VK_IMAGE_USAGE_SAMPLED_BIT)
#define TO_VK_FORMAT(format)        ((VkFormat)(format))
#define TO_VK_IMAGE_TYPE(type)      ((type) == ImageType::ImageCube ? VK_IMAGE_TYPE_2D : (VkImageType)(type))
#define TO_VK_IMAGE_VIEW_TYPE(type) ((VkImageViewType)(type))
#define TO_VK_IMAGE_ASPECT(usage)                                                                 \
    ((VkImageUsageFlags)((usage) & ImageUsage::DepthStencilAttachment ? VK_IMAGE_ASPECT_DEPTH_BIT \
                                                                      : VK_IMAGE_ASPECT_COLOR_BIT))
/* ^^^ TODO should support stencil and maybe multi-plane in the future */

extern VkDevice g_device;

namespace R3 {

Image::Image(usize3 extent, uint32 mipLevels, uint32 samples, ImageUsageFlags usage, Format format, ImageType type)
    : m_extent{extent},
      m_mipLevels{mipLevels},
      m_samples{samples},
      m_usage{usage},
      m_format{format},
      m_type{type} {
    const VkImageUsageFlags vkUsageFlags = TO_VK_USAGE_FLAGS(usage);
    const uint32 layerCount              = type == ImageType::ImageCube ? 6U : 1U;

    try {
        const VkImageCreateInfo imageInfo = {
            .sType     = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
            .flags     = type == ImageType::ImageCube ? VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT : 0U,
            .imageType = TO_VK_IMAGE_TYPE(type),
            .format    = TO_VK_FORMAT(format),
            .extent    = {static_cast<uint32>(extent.x), static_cast<uint32>(extent.y), static_cast<uint32>(extent.z)},
            .mipLevels = mipLevels,
            .arrayLayers           = type == ImageType::ImageCube ? 6U : 1U,
            .samples               = VkSampleCountFlagBits(samples),
            .tiling                = VK_IMAGE_TILING_OPTIMAL,
            .usage                 = vkUsageFlags,
            .sharingMode           = VK_SHARING_MODE_EXCLUSIVE,
            .queueFamilyIndexCount = 0,
            .pQueueFamilyIndices   = nullptr, /* only needed when sharingMode == VK_SHARING_MODE_CONCURRENT */
            .initialLayout         = VK_IMAGE_LAYOUT_UNDEFINED,
        };
        VK_CHECK(vkCreateImage(g_device, &imageInfo, nullptr, &m_image.get<VkImage>()));

        VkMemoryRequirements memoryRequirements;
        vkGetImageMemoryRequirements(g_device, m_image.get<VkImage>(), &memoryRequirements);

        const VkMemoryAllocateInfo memoryInfo = {
            .sType           = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
            .pNext           = nullptr,
            .allocationSize  = memoryRequirements.size,
            .memoryTypeIndex = GEngine()->RenderContext<vulkan::RenderContext>().queryDeviceMemoryTypeIndex(
                memoryRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT),
        };
        VK_CHECK(vkAllocateMemory(g_device, &memoryInfo, nullptr, &m_memory.get<VkDeviceMemory>()));
        VK_CHECK(vkBindImageMemory(g_device, m_image.get<VkImage>(), m_memory.get<VkDeviceMemory>(), 0));

        const VkImageViewCreateInfo imageViewInfo = {
            .sType      = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .image      = m_image.get<VkImage>(),
            .viewType   = TO_VK_IMAGE_VIEW_TYPE(type),
            .format     = imageInfo.format,
            .components = {},
            .subresourceRange =
                {
                    .aspectMask     = TO_VK_IMAGE_ASPECT(usage),
                    .baseMipLevel   = 0,
                    .levelCount     = imageInfo.mipLevels,
                    .baseArrayLayer = 0,
                    .layerCount     = layerCount,
                },
        };
        VK_CHECK(vkCreateImageView(g_device, &imageViewInfo, nullptr, &m_imageView.get<VkImageView>()));
    } catch (...) {
        this->~Image();
        throw;
    }
}

Image::~Image() {
    vkDestroyImage(g_device, m_image.get<VkImage>(), nullptr);
    vkFreeMemory(g_device, m_memory.get<VkDeviceMemory>(), nullptr);
    vkDestroyImageView(g_device, m_imageView.get<VkImageView>(), nullptr);
}

void Image::generateMipMaps(vulkan::CommandBuffer& cmd) {
    const VkImageAspectFlags aspectMask{TO_VK_IMAGE_ASPECT(m_usage)};
    const uint32 layerCount = m_type == ImageType::ImageCube ? 6U : 1U;
    uint32 mipLevels        = m_mipLevels;

    VkImageMemoryBarrier2 memoryBarrierWrite = {
        .sType               = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
        .srcStageMask        = VK_PIPELINE_STAGE_TRANSFER_BIT,
        .srcAccessMask       = VK_ACCESS_TRANSFER_WRITE_BIT,
        .dstStageMask        = VK_PIPELINE_STAGE_TRANSFER_BIT,
        .dstAccessMask       = VK_ACCESS_TRANSFER_READ_BIT,
        .oldLayout           = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        .newLayout           = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
        .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .image               = m_image.get<VkImage>(),
        .subresourceRange =
            {
                .aspectMask     = aspectMask,
                .baseMipLevel   = 0,
                .levelCount     = 1,
                .baseArrayLayer = 0,
                .layerCount     = layerCount,
            },
    };

    VkImageMemoryBarrier2 memoryBarrierShader = {
        .sType               = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
        .srcStageMask        = VK_PIPELINE_STAGE_TRANSFER_BIT,
        .srcAccessMask       = VK_ACCESS_TRANSFER_READ_BIT,
        .dstStageMask        = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
        .dstAccessMask       = VK_ACCESS_SHADER_READ_BIT,
        .oldLayout           = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
        .newLayout           = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
        .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .image               = m_image.get<VkImage>(),
        .subresourceRange =
            {
                .aspectMask     = aspectMask,
                .baseMipLevel   = 0,
                .levelCount     = 1,
                .baseArrayLayer = 0,
                .layerCount     = layerCount,
            },
    };

    const VkImageMemoryBarrier2 memoryBarrierFinal = {
        .sType               = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
        .srcStageMask        = VK_PIPELINE_STAGE_TRANSFER_BIT,
        .srcAccessMask       = VK_ACCESS_TRANSFER_WRITE_BIT,
        .dstStageMask        = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
        .dstAccessMask       = VK_ACCESS_SHADER_READ_BIT,
        .oldLayout           = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        .newLayout           = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
        .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .image               = m_image.get<VkImage>(),
        .subresourceRange =
            {
                .aspectMask     = aspectMask,
                .baseMipLevel   = mipLevels - 1,
                .levelCount     = 1,
                .baseArrayLayer = 0,
                .layerCount     = layerCount,
            },
    };

    // incremental mipmap generation
    // w and h are halved each iteration

    int32 w = static_cast<int32>(m_extent.x);
    int32 h = static_cast<int32>(m_extent.y);

    for (uint32 mipLevel = 0; mipLevel < mipLevels - 1; ++mipLevel) {
        memoryBarrierWrite.subresourceRange.baseMipLevel = mipLevel;
        cmd.pipelineBarrier({
            .sType                   = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
            .imageMemoryBarrierCount = 1,
            .pImageMemoryBarriers    = &memoryBarrierWrite,
        });
        memoryBarrierWrite.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED; // mipLevel 0 is DST_OPTIMAL, others are UNDEFINED

        const VkImageBlit2 blitRegion = {
            .sType = VK_STRUCTURE_TYPE_IMAGE_BLIT_2,
            .srcSubresource =
                {
                    .aspectMask     = aspectMask,
                    .mipLevel       = mipLevel,
                    .baseArrayLayer = 0,
                    .layerCount     = layerCount,
                },
            .srcOffsets =
                {
                    {0, 0, 0},
                    {w, h, 1},
                },
            .dstSubresource =
                {
                    .aspectMask     = aspectMask,
                    .mipLevel       = mipLevel + 1,
                    .baseArrayLayer = 0,
                    .layerCount     = layerCount,
                },
            .dstOffsets =
                {
                    {0, 0, 0},
                    {w > 1 ? w / 2 : 1, h > 1 ? h / 2 : 1, 1},
                },
        };

        cmd.blitImage({
            .sType          = VK_STRUCTURE_TYPE_BLIT_IMAGE_INFO_2,
            .srcImage       = m_image.get<VkImage>(),
            .srcImageLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
            .dstImage       = m_image.get<VkImage>(),
            .dstImageLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            .regionCount    = 1,
            .pRegions       = &blitRegion,
            .filter         = VK_FILTER_LINEAR,
        });

        memoryBarrierShader.subresourceRange.baseMipLevel = mipLevel;
        cmd.pipelineBarrier({
            .sType                   = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
            .imageMemoryBarrierCount = 1,
            .pImageMemoryBarriers    = &memoryBarrierShader,
        });

        w = w > 1 ? w / 2 : 1;
        h = h > 1 ? h / 2 : 1;
    }

    cmd.pipelineBarrier({
        .sType                   = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
        .imageMemoryBarrierCount = 1,
        .pImageMemoryBarriers    = &memoryBarrierFinal,
    });
}

} // namespace R3

#include "vulkan-Image.hpp"

#include <vulkan/vulkan_core.h>
#include "Exception.hpp"
#include "Types.hpp"
#include "vulkan-Check.hpp"
#include "vulkan-CommandBuffer.hpp"
#include "vulkan-Handle.hpp"
#include "vulkan-RenderContext.hpp"

namespace R3::vulkan {

Image::Image(RenderContext& ctx,
             VkFormat format,
             VkExtent2D extent,
             uint32 mipLevels,
             uint32 sampleCount,
             VkImageTiling tiling,
             VkImageUsageFlags usage,
             VkImageAspectFlags aspectFlags,
             VkMemoryPropertyFlags properties) {
    m_device    = ctx.device();
    m_extent    = extent;
    m_mipLevels = mipLevels;

    const VkImageCreateInfo imageInfo = {
        .sType                 = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
        .pNext                 = nullptr,
        .flags                 = {},
        .imageType             = VK_IMAGE_TYPE_2D,
        .format                = format,
        .extent                = {m_extent.width, m_extent.height, 1},
        .mipLevels             = m_mipLevels,
        .arrayLayers           = 1,
        .samples               = VkSampleCountFlagBits(sampleCount),
        .tiling                = tiling,
        .usage                 = usage,
        .sharingMode           = VK_SHARING_MODE_EXCLUSIVE,
        .queueFamilyIndexCount = 0,
        .pQueueFamilyIndices   = nullptr, /* would need this if using sharing mode concurrent */
        .initialLayout         = VK_IMAGE_LAYOUT_UNDEFINED,
    };
    VK_CHECK(vkCreateImage(m_device, &imageInfo, nullptr, &*m_image));

    VkMemoryRequirements memoryRequirements;
    vkGetImageMemoryRequirements(m_device, m_image, &memoryRequirements);

    const VkMemoryAllocateInfo memoryInfo = {
        .sType           = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        .pNext           = nullptr,
        .allocationSize  = memoryRequirements.size,
        .memoryTypeIndex = ctx.queryDeviceMemoryTypeIndex(memoryRequirements.memoryTypeBits, properties),
    };

    try {
        VK_CHECK(vkAllocateMemory(m_device, &memoryInfo, nullptr, &*m_imageMemory));
    } catch (const Exception& ex) {
        vkDestroyImage(m_device, m_image, nullptr);
        throw ex;
    }

    VK_CHECK(vkBindImageMemory(m_device, m_image, m_imageMemory, 0));

    const VkImageViewCreateInfo imageViewInfo = {
        .sType      = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
        .pNext      = nullptr,
        .flags      = {},
        .image      = m_image,
        .viewType   = VK_IMAGE_VIEW_TYPE_2D,
        .format     = format,
        .components = {},
        .subresourceRange =
            {
                .aspectMask     = aspectFlags,
                .baseMipLevel   = 0,
                .levelCount     = mipLevels,
                .baseArrayLayer = 0,
                .layerCount     = 1,
            },
    };
    VK_CHECK(vkCreateImageView(m_device, &imageViewInfo, nullptr, &*m_imageView));
}

Image::~Image() noexcept {
    if (m_device) {
        vkDestroyImage(m_device, m_image, nullptr);
        vkFreeMemory(m_device, m_imageMemory, nullptr);
        vkDestroyImageView(m_device, m_imageView, nullptr);
    }
}

void Image::generateMipMaps(CommandBuffer& cmd, VkQueue queue) {
    VkImageMemoryBarrier memoryBarrierWrite = {
        .sType               = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
        .pNext               = nullptr,
        .srcAccessMask       = VK_ACCESS_TRANSFER_WRITE_BIT,
        .dstAccessMask       = VK_ACCESS_TRANSFER_READ_BIT,
        .oldLayout           = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        .newLayout           = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
        .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .image               = m_image,
        .subresourceRange =
            {
                .aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT,
                .baseMipLevel   = 0,
                .levelCount     = 1,
                .baseArrayLayer = 0,
                .layerCount     = 1,
            },
    };

    VkImageMemoryBarrier memoryBarrierShader = {
        .sType               = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
        .pNext               = nullptr,
        .srcAccessMask       = VK_ACCESS_TRANSFER_READ_BIT,
        .dstAccessMask       = VK_ACCESS_SHADER_READ_BIT,
        .oldLayout           = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
        .newLayout           = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
        .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .image               = m_image,
        .subresourceRange =
            {
                .aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT,
                .baseMipLevel   = 0,
                .levelCount     = 1,
                .baseArrayLayer = 0,
                .layerCount     = 1,
            },
    };

    int32 w = static_cast<int32>(m_extent.width);
    int32 h = static_cast<int32>(m_extent.height);

    // incremental mipmap generation
    // w and h are halved each iteration
    for (uint32 i = 0; i < m_mipLevels - 1; ++i) {
        memoryBarrierWrite.subresourceRange.baseMipLevel = i;
        cmd.pipelineBarrier(
            VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, {}, {}, {&memoryBarrierWrite, 1});

        const VkImageBlit blitRegion = {
            .srcSubresource =
                {
                    .aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT,
                    .mipLevel       = i,
                    .baseArrayLayer = 0,
                    .layerCount     = 1,
                },
            .srcOffsets =
                {
                    {0, 0, 0},
                    {w, h, 1},
                },
            .dstSubresource =
                {
                    .aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT,
                    .mipLevel       = i + 1,
                    .baseArrayLayer = 0,
                    .layerCount     = 1,
                },
            .dstOffsets =
                {
                    {0, 0, 0},
                    {w > 1 ? w / 2 : 1, h > 1 ? h / 2 : 1, 1},
                },
        };

        cmd.blitImage(m_image,
                      VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                      m_image,
                      VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                      {&blitRegion, 1});

        memoryBarrierShader.subresourceRange.baseMipLevel = i;

        cmd.pipelineBarrier(VK_PIPELINE_STAGE_TRANSFER_BIT,
                            VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                            0,
                            {},
                            {},
                            {&memoryBarrierShader, 1});

        w = w > 1 ? w / 2 : 1;
        h = h > 1 ? h / 2 : 1;
    }

    const VkImageMemoryBarrier memoryBarrierFinal = {
        .sType               = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
        .pNext               = nullptr,
        .srcAccessMask       = VK_ACCESS_TRANSFER_WRITE_BIT,
        .dstAccessMask       = VK_ACCESS_SHADER_READ_BIT,
        .oldLayout           = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        .newLayout           = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
        .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .image               = m_image,
        .subresourceRange =
            {
                .aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT,
                .baseMipLevel   = m_mipLevels - 1,
                .levelCount     = 1,
                .baseArrayLayer = 0,
                .layerCount     = 1,
            },
    };
    cmd.pipelineBarrier(
        VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, {}, {}, {&memoryBarrierFinal, 1});
}

} // namespace R3::vulkan

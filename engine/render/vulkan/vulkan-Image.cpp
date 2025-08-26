#include "vulkan-Image.hpp"

#include <vulkan/vulkan_core.h>
#include "api/Exception.hpp"
#include "api/Types.hpp"
#include "core/Engine.hpp"
#include "vulkan-Check.hpp"
#include "vulkan-CommandBuffer.hpp"
#include "vulkan-Handle.hpp"
#include "vulkan-RenderContext.hpp"

namespace R3::vulkan {

Image::Image(VkFormat format,
             VkExtent2D extent,
             uint32 mipLevels,
             VkSampleCountFlagBits sampleCount,
             VkImageTiling tiling,
             VkImageUsageFlags usage,
             VkImageAspectFlags aspectFlags,
             VkMemoryPropertyFlags properties) {
    RenderContext& ctx = static_cast<RenderContext&>(Engine()->context());

    try {
        const VkImageCreateInfo imageInfo = {
            .sType                 = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
            .pNext                 = nullptr,
            .flags                 = {},
            .imageType             = VK_IMAGE_TYPE_2D,
            .format                = format,
            .extent                = {extent.width, extent.height, 1},
            .mipLevels             = mipLevels,
            .arrayLayers           = 1,
            .samples               = sampleCount,
            .tiling                = tiling,
            .usage                 = usage,
            .sharingMode           = VK_SHARING_MODE_EXCLUSIVE,
            .queueFamilyIndexCount = 0,
            .pQueueFamilyIndices   = nullptr, /* would need this if using sharing mode concurrent */
            .initialLayout         = VK_IMAGE_LAYOUT_UNDEFINED,
        };
        VK_CHECK(vkCreateImage(ctx.device(), &imageInfo, nullptr, &*m_image));

        VkMemoryRequirements memoryRequirements;
        vkGetImageMemoryRequirements(ctx.device(), m_image, &memoryRequirements);

        const VkMemoryAllocateInfo memoryInfo = {
            .sType           = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
            .pNext           = nullptr,
            .allocationSize  = memoryRequirements.size,
            .memoryTypeIndex = ctx.queryDeviceMemoryTypeIndex(memoryRequirements.memoryTypeBits, properties),
        };
        VK_CHECK(vkAllocateMemory(ctx.device(), &memoryInfo, nullptr, &*m_imageMemory));
        VK_CHECK(vkBindImageMemory(ctx.device(), m_image, m_imageMemory, 0));

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
        VK_CHECK(vkCreateImageView(ctx.device(), &imageViewInfo, nullptr, &*m_imageView));
    } catch (const Exception& ex) {
        vkDestroyImage(ctx.device(), m_image, nullptr);
        vkFreeMemory(ctx.device(), m_imageMemory, nullptr);
        vkDestroyImageView(ctx.device(), m_imageView, nullptr);
        throw ex;
    }
}

Image::~Image() noexcept {
    RenderContext& ctx = static_cast<RenderContext&>(Engine()->context());
    vkDestroyImage(ctx.device(), m_image, nullptr);
    vkFreeMemory(ctx.device(), m_imageMemory, nullptr);
    vkDestroyImageView(ctx.device(), m_imageView, nullptr);
}

void Image::generateMipMaps(CommandBuffer& cmd, VkExtent2D extent, uint32 mipLevels) {
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

    int32 w = static_cast<int32>(extent.width);
    int32 h = static_cast<int32>(extent.height);

    // incremental mipmap generation
    // w and h are halved each iteration
    for (uint32 i = 0; i < mipLevels - 1; ++i) {
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
                .baseMipLevel   = mipLevels - 1,
                .levelCount     = 1,
                .baseArrayLayer = 0,
                .layerCount     = 1,
            },
    };
    cmd.pipelineBarrier(
        VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, {}, {}, {&memoryBarrierFinal, 1});
}

} // namespace R3::vulkan

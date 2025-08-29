#include "vulkan-Image.hpp"

#include <algorithm>
#include <volk.h>
#include "api/Exception.hpp"
#include "api/Types.hpp"
#include "core/Engine.hpp"
#include "vulkan-Check.hpp"
#include "vulkan-CommandBuffer.hpp"
#include "vulkan-Handle.hpp"
#include "vulkan-RenderContext.hpp"

namespace R3::vulkan {

Image::Image(VkImageCreateInfo imageInfo, VkImageAspectFlags aspectFlags, VkMemoryPropertyFlags properties) {
    RenderContext& ctx = static_cast<RenderContext&>(Engine()->context());

    try {
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
            .viewType   = (imageInfo.flags & VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT) ? VK_IMAGE_VIEW_TYPE_CUBE
                                                                                  : VK_IMAGE_VIEW_TYPE_2D,
            .format     = imageInfo.format,
            .components = {},
            .subresourceRange =
                {
                    .aspectMask     = aspectFlags,
                    .baseMipLevel   = 0,
                    .levelCount     = imageInfo.mipLevels,
                    .baseArrayLayer = 0,
                    .layerCount     = (uint32)((imageInfo.flags & VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT) ? 6 : 1),
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

void Image::generateMipMaps(CommandBuffer& cmd, VkExtent2D extent, uint32 mipLevels, uint32 layerCount) {
    VkImageMemoryBarrier2 memoryBarrierWrite = {
        .sType               = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
        .pNext               = nullptr,
        .srcStageMask        = VK_PIPELINE_STAGE_TRANSFER_BIT,
        .srcAccessMask       = VK_ACCESS_TRANSFER_WRITE_BIT,
        .dstStageMask        = VK_PIPELINE_STAGE_TRANSFER_BIT,
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

    VkImageMemoryBarrier2 memoryBarrierShader = {
        .sType               = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
        .pNext               = nullptr,
        .srcStageMask        = VK_PIPELINE_STAGE_TRANSFER_BIT,
        .srcAccessMask       = VK_ACCESS_TRANSFER_READ_BIT,
        .dstStageMask        = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
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

    // incremental mipmap generation
    // w and h are halved each iteration
    for (uint32 layer = 0; layer < layerCount; ++layer) {
        int32 w = static_cast<int32>(extent.width);
        int32 h = static_cast<int32>(extent.height);

        for (uint32 mipLevel = 0; mipLevel < mipLevels - 1; ++mipLevel) {
            memoryBarrierWrite.subresourceRange.baseMipLevel   = mipLevel;
            memoryBarrierWrite.subresourceRange.baseArrayLayer = layer;
            cmd.transitionImageLayout(memoryBarrierWrite);

            const VkImageBlit blitRegion = {
                .srcSubresource =
                    {
                        .aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT,
                        .mipLevel       = mipLevel,
                        .baseArrayLayer = layer,
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
                        .mipLevel       = mipLevel + 1,
                        .baseArrayLayer = layer,
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

            memoryBarrierShader.subresourceRange.baseMipLevel   = mipLevel;
            memoryBarrierShader.subresourceRange.baseArrayLayer = layer;

            cmd.transitionImageLayout(memoryBarrierShader);

            w = w > 1 ? w / 2 : 1;
            h = h > 1 ? h / 2 : 1;
        }
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
                .layerCount     = layerCount,
            },
    };
    cmd.pipelineBarrier(
        VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, {}, {}, {&memoryBarrierFinal, 1});
}

} // namespace R3::vulkan

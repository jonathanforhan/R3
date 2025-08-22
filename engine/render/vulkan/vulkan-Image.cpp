#if R3_VULKAN

#include "vulkan-Image.hpp"

#include <vulkan/vulkan_core.h>
#include "Exception.hpp"
#include "Types.hpp"
#include "vulkan-Check.hpp"
#include "vulkan-RenderContext.hpp"

namespace R3::vulkan {

void Image::create(RenderContext& ctx,
                   VkFormat format,
                   VkExtent3D extent,
                   uint32 mipLevels,
                   uint32 sampleCount,
                   VkImageTiling tiling,
                   VkImageUsageFlags usage,
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
        .extent                = m_extent,
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
    VK_CHECK(vkCreateImage(m_device, &imageInfo, nullptr, &m_image));

    VkMemoryRequirements memoryRequirements;
    vkGetImageMemoryRequirements(m_device, m_image, &memoryRequirements);

    const VkMemoryAllocateInfo memoryInfo = {
        .sType           = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        .pNext           = nullptr,
        .allocationSize  = memoryRequirements.size,
        .memoryTypeIndex = ctx.deviceMemoryTypeIndex(memoryRequirements.memoryTypeBits, properties),
    };

    try {
        VK_CHECK(vkAllocateMemory(m_device, &memoryInfo, nullptr, &m_imageMemory));
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
                .aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT,
                .baseMipLevel   = 0,
                .levelCount     = mipLevels,
                .baseArrayLayer = 0,
                .layerCount     = 1,
            },
    };
    VK_CHECK(vkCreateImageView(m_device, &imageViewInfo, nullptr, &m_imageView));
}

void Image::destroy() noexcept {
    if (m_device != nullptr) {
        if (m_image != VK_NULL_HANDLE) {
            vkDestroyImage(m_device, m_image, nullptr);
            m_image = VK_NULL_HANDLE;
        }

        if (m_imageMemory != VK_NULL_HANDLE) {
            vkFreeMemory(m_device, m_imageMemory, nullptr);
            m_imageMemory = VK_NULL_HANDLE;
        }

        vkDestroyImageView(m_device, m_imageView, nullptr);
        m_imageView = VK_NULL_HANDLE;
    }
    m_device = VK_NULL_HANDLE;

    m_extent = {};
}

void Image::copy(VkCommandBuffer cmd, VkQueue queue, VkBuffer buffer, const VkBufferImageCopy& bufferToImage) {
    const VkCommandBufferBeginInfo cmdInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
    };
    VK_CHECK(vkBeginCommandBuffer(cmd, &cmdInfo));

    vkCmdCopyBufferToImage(cmd, buffer, m_image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &bufferToImage);

    VK_CHECK(vkEndCommandBuffer(cmd));
    const VkSubmitInfo submitInfo = {
        .sType              = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .commandBufferCount = 1,
        .pCommandBuffers    = &cmd,
    };
    VK_CHECK(vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE));
    VK_CHECK(vkQueueWaitIdle(queue));
}

void Image::copy(VkCommandBuffer cmd, VkQueue queue, VkImage image, const VkImageCopy& imageToImage) {
    const VkCommandBufferBeginInfo cmdInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
    };
    VK_CHECK(vkBeginCommandBuffer(cmd, &cmdInfo));

    vkCmdCopyImage(cmd,
                   image,
                   VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                   m_image,
                   VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                   1,
                   &imageToImage);

    VK_CHECK(vkEndCommandBuffer(cmd));
    const VkSubmitInfo submitInfo = {
        .sType              = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .commandBufferCount = 1,
        .pCommandBuffers    = &cmd,
    };
    VK_CHECK(vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE));
    VK_CHECK(vkQueueWaitIdle(queue));
}

void Image::transition(VkCommandBuffer cmd,
                       VkQueue queue,
                       VkPipelineStageFlags srcStage,
                       VkPipelineStageFlags dstStage,
                       const VkImageMemoryBarrier& memoryBarrier) {
    const VkCommandBufferBeginInfo cmdInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
    };
    VK_CHECK(vkBeginCommandBuffer(cmd, &cmdInfo));

    vkCmdPipelineBarrier(cmd, srcStage, dstStage, 0, 0, nullptr, 0, nullptr, 1, &memoryBarrier);

    VK_CHECK(vkEndCommandBuffer(cmd));
    const VkSubmitInfo submitInfo = {
        .sType              = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .commandBufferCount = 1,
        .pCommandBuffers    = &cmd,
    };
    VK_CHECK(vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE));
    VK_CHECK(vkQueueWaitIdle(queue));
}

void Image::generateMipMaps(VkCommandBuffer cmd, VkQueue queue) {
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
        transition(cmd, queue, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, memoryBarrierWrite);

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

        w = w > 1 ? w / 2 : 1;
        h = h > 1 ? h / 2 : 1;

        const VkCommandBufferBeginInfo cmdInfo = {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
            .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
        };
        VK_CHECK(vkBeginCommandBuffer(cmd, &cmdInfo));

        vkCmdBlitImage(cmd,
                       m_image,
                       VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                       m_image,
                       VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                       1,
                       &blitRegion,
                       VK_FILTER_LINEAR);

        VK_CHECK(vkEndCommandBuffer(cmd));
        const VkSubmitInfo submitInfo = {
            .sType              = VK_STRUCTURE_TYPE_SUBMIT_INFO,
            .commandBufferCount = 1,
            .pCommandBuffers    = &cmd,
        };
        VK_CHECK(vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE));
        VK_CHECK(vkQueueWaitIdle(queue));

        memoryBarrierShader.subresourceRange.baseMipLevel = i;
        transition(
            cmd, queue, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, memoryBarrierShader);
    }
}

} // namespace R3::vulkan

#endif // R3_VULKAN
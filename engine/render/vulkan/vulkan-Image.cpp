#if R3_VULKAN

#include "vulkan-Image.hpp"

#include <vulkan/vulkan_core.h>
#include "Assert.hpp"
#include "Exception.hpp"
#include "Types.hpp"
#include "vulkan-Check.hpp"
#include "vulkan-RenderContext.hpp"

namespace R3::vulkan {

void Image::allocate(RenderContext& ctx,
                     VkFormat format,
                     VkExtent3D extent,
                     uint32 mipLevels,
                     uint32 sampleCount,
                     VkImageTiling tiling,
                     VkImageUsageFlags usage,
                     VkMemoryPropertyFlags properties) {
    m_ctx    = &ctx;
    m_extent = extent;

    const VkImageCreateInfo imageInfo = {
        .sType                 = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
        .pNext                 = nullptr,
        .flags                 = {},
        .imageType             = VK_IMAGE_TYPE_2D,
        .format                = format,
        .extent                = m_extent,
        .mipLevels             = mipLevels,
        .arrayLayers           = 1,
        .samples               = VkSampleCountFlagBits(sampleCount),
        .tiling                = tiling,
        .usage                 = usage,
        .sharingMode           = VK_SHARING_MODE_EXCLUSIVE,
        .queueFamilyIndexCount = 0,
        .pQueueFamilyIndices   = nullptr, /* would need this if using sharing mode concurrent */
        .initialLayout         = VK_IMAGE_LAYOUT_UNDEFINED,
    };
    VK_CHECK(vkCreateImage(m_ctx->device(), &imageInfo, nullptr, &m_image));

    VkMemoryRequirements memoryRequirements;
    vkGetImageMemoryRequirements(m_ctx->device(), m_image, &memoryRequirements);

    const VkMemoryAllocateInfo memoryInfo = {
        .sType           = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        .pNext           = nullptr,
        .allocationSize  = memoryRequirements.size,
        .memoryTypeIndex = m_ctx->deviceMemoryTypeIndex(memoryRequirements.memoryTypeBits, properties),
    };

    try {
        VK_CHECK(vkAllocateMemory(m_ctx->device(), &memoryInfo, nullptr, &m_imageMemory));
    } catch (const Exception& ex) {
        vkDestroyImage(m_ctx->device(), m_image, nullptr);
        throw ex;
    }

    VK_CHECK(vkBindImageMemory(m_ctx->device(), m_image, m_imageMemory, 0));

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
    VK_CHECK(vkCreateImageView(m_ctx->device(), &imageViewInfo, nullptr, &m_imageView));
}

void Image::free() noexcept {
    if (m_mappedMemory) {
        unmap();
    }

    if (m_ctx != nullptr) {
        if (m_image != VK_NULL_HANDLE) {
            vkDestroyImage(m_ctx->device(), m_image, nullptr);
            m_image = VK_NULL_HANDLE;
        }

        if (m_imageMemory != VK_NULL_HANDLE) {
            vkFreeMemory(m_ctx->device(), m_imageMemory, nullptr);
            m_imageMemory = VK_NULL_HANDLE;
        }

        vkDestroyImageView(m_ctx->device(), m_imageView, nullptr);
        m_imageView = VK_NULL_HANDLE;
    }
    m_ctx = nullptr;

    m_extent = {};
}

void Image::copy(VkCommandBuffer cmd, VkBuffer buffer, const VkBufferImageCopy& bufferToImage) {
    R3_ASSERT(cmd);

    const VkCommandBufferBeginInfo cmdInfo = {
        .sType            = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .pNext            = nullptr,
        .flags            = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
        .pInheritanceInfo = nullptr,
    };
    VK_CHECK(vkBeginCommandBuffer(cmd, &cmdInfo));

    vkCmdCopyBufferToImage(cmd, buffer, m_image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &bufferToImage);

    VK_CHECK(vkEndCommandBuffer(cmd));

    const VkSubmitInfo submitInfo = {
        .sType                = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .pNext                = nullptr,
        .waitSemaphoreCount   = 0,
        .pWaitSemaphores      = nullptr,
        .pWaitDstStageMask    = nullptr,
        .commandBufferCount   = 1,
        .pCommandBuffers      = &cmd,
        .signalSemaphoreCount = 0,
        .pSignalSemaphores    = nullptr,
    };
    VK_CHECK(vkQueueSubmit(m_ctx->graphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE));

    VK_CHECK(vkQueueWaitIdle(m_ctx->graphicsQueue()));
}

void Image::copy(VkCommandBuffer cmd, VkImage image, const VkImageCopy& imageToImage) {
    R3_ASSERT(cmd);

    const VkCommandBufferBeginInfo cmdInfo = {
        .sType            = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .pNext            = nullptr,
        .flags            = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
        .pInheritanceInfo = nullptr,
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
        .sType                = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .pNext                = nullptr,
        .waitSemaphoreCount   = 0,
        .pWaitSemaphores      = nullptr,
        .pWaitDstStageMask    = nullptr,
        .commandBufferCount   = 1,
        .pCommandBuffers      = &cmd,
        .signalSemaphoreCount = 0,
        .pSignalSemaphores    = nullptr,
    };
    VK_CHECK(vkQueueSubmit(m_ctx->graphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE));

    VK_CHECK(vkQueueWaitIdle(m_ctx->graphicsQueue()));
}

void Image::generateMipMaps() {}

} // namespace R3::vulkan

#endif // R3_VULKAN
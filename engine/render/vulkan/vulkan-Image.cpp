#if R3_VULKAN

#include "render/Image.hpp"

#include <vulkan/vulkan_core.h>
#include "Exception.hpp"
#include "Types.hpp"
#include "render/Flags.hpp"
#include "render/RenderContext.hpp"
#include "vulkan-Check.hpp"

namespace R3 {

void Image::allocate(RenderContext& ctx,
                     VkFormat format,
                     uvec2 extent,
                     uint32 mipLevels,
                     uint32 sampleCount,
                     ImageTiling tiling,
                     ImageUsage usage,
                     MemoryProperties properties) {
    m_device         = ctx.device();
    m_physicalDevice = ctx.physicalDevice();
    m_extent         = extent;

    const VkImageCreateInfo imageInfo = {
        .sType     = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
        .pNext     = nullptr,
        .flags     = {},
        .imageType = VK_IMAGE_TYPE_2D,
        .format    = format,
        .extent =
            {
                .width  = m_extent.x,
                .height = m_extent.y,
                .depth  = 1,
            },
        .mipLevels             = mipLevels,
        .arrayLayers           = 1,
        .samples               = VkSampleCountFlagBits(sampleCount),
        .tiling                = VkImageTiling(tiling),
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

void Image::free() noexcept {
    if (m_mappedMemory) {
        unmap();
    }

    if (m_image != VK_NULL_HANDLE && m_device != VK_NULL_HANDLE) {
        vkDestroyImage(m_device, m_image, nullptr);
        m_image = VK_NULL_HANDLE;
    }

    if (m_imageMemory != VK_NULL_HANDLE && m_device != VK_NULL_HANDLE) {
        vkFreeMemory(m_device, m_imageMemory, nullptr);
        m_imageMemory = VK_NULL_HANDLE;
    }

    vkDestroyImageView(m_device, m_imageView, nullptr);
    vkDestroyImage(m_device, m_image, nullptr);
}

void Image::copy(const void* src, usize size) {}

void Image::generateMipMaps() {}

} // namespace R3

#endif
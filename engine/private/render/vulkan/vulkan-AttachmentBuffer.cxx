#if R3_VULKAN

#include "vulkan-AttachmentBuffer.hxx"

#include "vulkan/vulkan.h"
#include "vulkan-LogicalDevice.hxx"
#include "vulkan-PhysicalDevice.hxx"
#include "vulkan-VulkanObject.hxx"
#include <api/Assert.hpp>
#include <api/Types.hpp>

namespace R3::vulkan {

R3::vulkan::AttachmentBuffer::AttachmentBuffer(const AttachmentBufferSpecification& spec)
    : m_device(spec.device),
      m_format(spec.format),
      m_extent(spec.extent),
      m_sampleCount(spec.sampleCount) {
    // this constructor must
    // [ ] create image
    // [ ] allocate device memory
    // [ ] create image view

    // image creation
    const uint32 indices[] = {
        spec.device.graphicsQueue().index(),
        spec.device.presentationQueue().index(),
    };

    const VkImageCreateInfo imageCreateInfo = {
        .sType     = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
        .pNext     = nullptr,
        .flags     = {},
        .imageType = VK_IMAGE_TYPE_2D,
        .format    = m_format,
        .extent =
            {
                .width  = m_extent.width,
                .height = m_extent.height,
                .depth  = 1,
            },
        .mipLevels             = 1,
        .arrayLayers           = 1,
        .samples               = m_sampleCount,
        .tiling                = VK_IMAGE_TILING_OPTIMAL,
        .usage                 = spec.imageUsage,
        .sharingMode           = VK_SHARING_MODE_EXCLUSIVE,
        .queueFamilyIndexCount = static_cast<uint32>(std::size(indices)),
        .pQueueFamilyIndices   = indices,
        .initialLayout         = VK_IMAGE_LAYOUT_UNDEFINED,
    };

    VkImage image;
    VkResult result = vkCreateImage(m_device, &imageCreateInfo, nullptr, &image);
    ENSURE(result == VK_SUCCESS);
    /* [X] create image */ VulkanObject<VkImage>::m_handle = image;

    // allocate device memory
    VkMemoryRequirements memoryRequirements;
    vkGetImageMemoryRequirements(m_device, image, &memoryRequirements);

    const VkMemoryAllocateInfo memoryAllocateInfo = {
        .sType           = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        .pNext           = nullptr,
        .allocationSize  = memoryRequirements.size,
        .memoryTypeIndex = spec.physicalDevice.queryMemoryType(memoryRequirements.memoryTypeBits, spec.memoryProperty),
    };

    VkDeviceMemory memory;
    result = vkAllocateMemory(m_device, &memoryAllocateInfo, nullptr, &memory);
    ENSURE(result == VK_SUCCESS);
    /* [X] allocate device memory */ VulkanObject<VkDeviceMemory>::m_handle = memory;

    vkBindImageMemory(m_device, image, memory, 0);

    const VkImageViewCreateInfo imageViewCreateInfo = {
        .sType    = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
        .pNext    = nullptr,
        .flags    = {},
        .image    = image,
        .viewType = VK_IMAGE_VIEW_TYPE_2D,
        .format   = m_format,
        .components =
            {
                .r = VK_COMPONENT_SWIZZLE_IDENTITY,
                .g = VK_COMPONENT_SWIZZLE_IDENTITY,
                .b = VK_COMPONENT_SWIZZLE_IDENTITY,
                .a = VK_COMPONENT_SWIZZLE_IDENTITY,
            },
        .subresourceRange =
            {
                .aspectMask     = spec.aspectFlags,
                .baseMipLevel   = 0,
                .levelCount     = 1,
                .baseArrayLayer = 0,
                .layerCount     = 1,
            },
    };

    VkImageView imageView;
    result = vkCreateImageView(m_device, &imageViewCreateInfo, nullptr, &imageView);
    ENSURE(result == VK_SUCCESS);
    /* [X] create image view */ VulkanObject<VkImageView>::m_handle = imageView;
}

AttachmentBuffer::~AttachmentBuffer() {
    vkDestroyImageView(m_device, VulkanObject<VkImageView>::m_handle, nullptr);
    vkDestroyImage(m_device, VulkanObject<VkImage>::m_handle, nullptr);
    vkFreeMemory(m_device, VulkanObject<VkDeviceMemory>::m_handle, nullptr);
}

void AttachmentBuffer::free() {
    if (m_device) {
        vkDestroyImageView(m_device, VulkanObject<VkImageView>::m_handle, nullptr);
        vkDestroyImage(m_device, VulkanObject<VkImage>::m_handle, nullptr);
        vkFreeMemory(m_device, VulkanObject<VkDeviceMemory>::m_handle, nullptr);
    }

    VulkanObject<VkImageView>::m_handle    = VK_NULL_HANDLE;
    VulkanObject<VkImage>::m_handle        = VK_NULL_HANDLE;
    VulkanObject<VkDeviceMemory>::m_handle = VK_NULL_HANDLE;
}

} // namespace R3::vulkan

#endif // R3_VULKAN

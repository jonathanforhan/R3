#if R3_VULKAN

#include "vulkan-AttachmentBuffer.hxx"

#include "vulkan-LogicalDevice.hxx"
#include "vulkan-PhysicalDevice.hxx"
#include "vulkan-VulkanObject.hxx"
#include <api/Log.hpp>
#include <api/Result.hpp>
#include <api/Types.hpp>
#include <expected>
#include <vulkan/vulkan.h>

namespace R3::vulkan {

Result<AttachmentBuffer> AttachmentBuffer::create(const AttachmentBufferSpecification& spec) {
    // A note on error handling:
    // The early returns look scary, however keep in mind that when the resource is assigned to self it is now part of
    // RAII clean up and will be cleaned up by the destructor on an early return because it will go out of scope when
    // std::unexpected is returned.

    AttachmentBuffer self;
    self.m_device = spec.device.vk();

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
        .format    = spec.format,
        .extent =
            {
                .width  = spec.extent.width,
                .height = spec.extent.height,
                .depth  = 1,
            },
        .mipLevels             = spec.mipLevels,
        .arrayLayers           = 1,
        .samples               = spec.sampleCount,
        .tiling                = VK_IMAGE_TILING_OPTIMAL,
        .usage                 = spec.imageUsage,
        .sharingMode           = VK_SHARING_MODE_EXCLUSIVE,
        .queueFamilyIndexCount = static_cast<uint32>(std::size(indices)),
        .pQueueFamilyIndices   = indices,
        .initialLayout         = VK_IMAGE_LAYOUT_UNDEFINED,
    };

    VkImage image;
    VkResult result = vkCreateImage(self.m_device, &imageCreateInfo, nullptr, &image);
    if (result != VK_SUCCESS) {
        R3_LOG(Error, "vkCreateImage failure {}", (int)result);
        return std::unexpected(Error::InitializationFailure);
    }
    self.VulkanObject<VkImage>::m_handle = image;
    /* [X] create image */

    // allocate device memory
    VkMemoryRequirements memoryRequirements;
    vkGetImageMemoryRequirements(self.m_device, image, &memoryRequirements);

    Result<uint32> memoryTypeIndex =
        spec.physicalDevice.queryMemoryType(memoryRequirements.memoryTypeBits, spec.memoryProperty);
    R3_PROPAGATE(memoryTypeIndex);

    const VkMemoryAllocateInfo memoryAllocateInfo = {
        .sType           = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        .pNext           = nullptr,
        .allocationSize  = memoryRequirements.size,
        .memoryTypeIndex = memoryTypeIndex.value(),
    };

    VkDeviceMemory memory;
    result = vkAllocateMemory(self.m_device, &memoryAllocateInfo, nullptr, &memory);
    if (result != VK_SUCCESS) {
        R3_LOG(Error, "vkAllocateMemory failure {}", (int)result);
        return std::unexpected(Error::AllocationFailure);
    }
    self.VulkanObject<VkDeviceMemory>::m_handle = memory;
    /* [X] allocate device memory */

    result = vkBindImageMemory(self.m_device, image, memory, 0);
    if (result != VK_SUCCESS) {
        R3_LOG(Error, "vkBindImageMemory failure {}", (int)result);
        return std::unexpected(Error::InitializationFailure);
    }

    const VkImageViewCreateInfo imageViewCreateInfo = {
        .sType    = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
        .pNext    = nullptr,
        .flags    = {},
        .image    = image,
        .viewType = VK_IMAGE_VIEW_TYPE_2D,
        .format   = spec.format,
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
                .levelCount     = spec.mipLevels,
                .baseArrayLayer = 0,
                .layerCount     = 1,
            },
    };

    VkImageView imageView;
    result = vkCreateImageView(self.m_device, &imageViewCreateInfo, nullptr, &imageView);
    if (result != VK_SUCCESS) {
        R3_LOG(Error, "vkCreateImageView failure {}", (int)result);
        return std::unexpected(Error::InitializationFailure);
    }
    self.VulkanObject<VkImageView>::m_handle = imageView;
    /* [X] create image view */

    return self;
}

AttachmentBuffer::~AttachmentBuffer() {
    vkDestroyImageView(m_device, VulkanObject<VkImageView>::m_handle, nullptr);
    vkDestroyImage(m_device, VulkanObject<VkImage>::m_handle, nullptr);
    vkFreeMemory(m_device, VulkanObject<VkDeviceMemory>::m_handle, nullptr);
}

void AttachmentBuffer::free() {
    if (m_device) { // check for m_device because it might be used with invalid buffer on swapchain recreation
        vkDestroyImageView(m_device, VulkanObject<VkImageView>::m_handle, nullptr);
        vkDestroyImage(m_device, VulkanObject<VkImage>::m_handle, nullptr);
        vkFreeMemory(m_device, VulkanObject<VkDeviceMemory>::m_handle, nullptr);
    }
}

constexpr bool AttachmentBuffer::valid() const {
    return VulkanObject<VkImage>::valid() && VulkanObject<VkImageView>::valid() &&
           VulkanObject<VkDeviceMemory>::valid();
}

} // namespace R3::vulkan

#endif // R3_VULKAN

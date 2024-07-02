#if R3_VULKAN

#include "vulkan-AttachmentBuffer.hxx"

#include "vulkan-LogicalDevice.hxx"
#include "vulkan-PhysicalDevice.hxx"
#include "vulkan-VulkanObject.hxx"
#include <api/Log.hpp>
#include <api/Result.hpp>
#include <api/Types.hpp>
#include <cassert>
#include <expected>
#include <vulkan/vulkan.h>

namespace R3::vulkan {

Result<void> AttachmentBuffer::initialize(const AttachmentBufferSpecification& spec) {
    // A note on error handling:
    // The early returns look scary, however keep in mind that when the resource is assigned to self it is now part of
    // RAII clean up and will be cleaned up by the destructor on an early return because it will go out of scope when
    // std::unexpected is returned.

    m_device = spec.device.vk();

    // this constructor must
    // [ ] create image
    // [ ] allocate device memory
    // [ ] create image view

    // image creation
    VkImage image;
    if (VkResult result = vkCreateImage(m_device, &spec.imageCreateInfo, nullptr, &image)) {
        R3_LOG(Error, "vkCreateImage failure {}", (int)result);
        return std::unexpected(Error::InitializationFailure);
    }
    VulkanObject<VkImage>::m_handle = image;
    /* [X] create image */

    // allocate device memory
    VkMemoryRequirements memoryRequirements;
    vkGetImageMemoryRequirements(m_device, image, &memoryRequirements);

    const VkMemoryAllocateInfo memoryAllocateInfo = {
        .sType          = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        .pNext          = nullptr,
        .allocationSize = memoryRequirements.size,
        .memoryTypeIndex =
            spec.physicalDevice.queryMemoryType(memoryRequirements.memoryTypeBits, spec.memoryPropertyFlags),
    };

    VkDeviceMemory memory;
    if (VkResult result = vkAllocateMemory(m_device, &memoryAllocateInfo, nullptr, &memory)) {
        R3_LOG(Error, "vkAllocateMemory failure {}", (int)result);
        return std::unexpected(Error::AllocationFailure);
    }
    VulkanObject<VkDeviceMemory>::m_handle = memory;
    /* [X] allocate device memory */

    if (VkResult result = vkBindImageMemory(m_device, image, memory, 0)) {
        R3_LOG(Error, "vkBindImageMemory failure {}", (int)result);
        return std::unexpected(Error::InitializationFailure);
    }

    assert(spec.imageViewCreateInfo.image == VK_NULL_HANDLE);
    spec.imageViewCreateInfo.image = image; // change image to newly created image

    VkImageView imageView;
    if (VkResult result = vkCreateImageView(m_device, &spec.imageViewCreateInfo, nullptr, &imageView)) {
        R3_LOG(Error, "vkCreateImageView failure {}", (int)result);
        return std::unexpected(Error::InitializationFailure);
    }
    VulkanObject<VkImageView>::m_handle = imageView;
    /* [X] create image view */

    return Result<void>();
}

AttachmentBuffer::~AttachmentBuffer() {
    vkDestroyImageView(m_device, VulkanObject<VkImageView>::m_handle, nullptr);
    vkDestroyImage(m_device, VulkanObject<VkImage>::m_handle, nullptr);
    vkFreeMemory(m_device, VulkanObject<VkDeviceMemory>::m_handle, nullptr);
}

void AttachmentBuffer::free() {
    vkDestroyImageView(m_device, VulkanObject<VkImageView>::m_handle, nullptr);
    vkDestroyImage(m_device, VulkanObject<VkImage>::m_handle, nullptr);
    vkFreeMemory(m_device, VulkanObject<VkDeviceMemory>::m_handle, nullptr);
}

constexpr bool AttachmentBuffer::valid() const {
    return VulkanObject<VkImage>::valid() && VulkanObject<VkImageView>::valid() &&
           VulkanObject<VkDeviceMemory>::valid();
}

} // namespace R3::vulkan

#endif // R3_VULKAN

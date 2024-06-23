#pragma once

#if R3_VULKAN

#include "vulkan-fwd.hxx"
#include "vulkan-VulkanObject.hxx"
#include <api/Construct.hpp>
#include <api/Types.hpp>
#include <vulkan/vulkan.h>

namespace R3::vulkan {

struct AttachmentBufferSpecification {
    const PhysicalDevice& physicalDevice;
    const LogicalDevice& device;
    VkFormat format;
    VkExtent2D extent;
    VkSampleCountFlagBits sampleCount;
    VkImageAspectFlags aspectFlags;
    VkImageUsageFlags imageUsage;
    VkMemoryPropertyFlags memoryProperty;
    uint32 mipLevels = 1;
};

class AttachmentBuffer : public VulkanObject<VkImage>,
                         public VulkanObject<VkImageView>,
                         public VulkanObject<VkDeviceMemory> {
public:
    using VulkanObject<VkImage>::vk;

protected:
    DEFAULT_CONSTRUCT(AttachmentBuffer);
    NO_COPY(AttachmentBuffer);
    DEFAULT_MOVE(AttachmentBuffer);

    AttachmentBuffer(const AttachmentBufferSpecification& spec);

    ~AttachmentBuffer();

public:
    void free();

    constexpr VkImageView imageView() const { return VulkanObject<VkImageView>::vk(); }

protected:
    VkDevice m_device = VK_NULL_HANDLE;
};

} // namespace R3::vulkan

#endif // R3_VULKAN

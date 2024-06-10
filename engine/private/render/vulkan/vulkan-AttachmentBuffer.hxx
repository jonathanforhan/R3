#pragma once

#if R3_VULKAN

#include "api/Construct.hpp"
#include "vulkan-fwd.hxx"
#include "vulkan-VulkanObject.hxx"
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
protected:
    DEFAULT_CONSTRUCT(AttachmentBuffer);
    NO_COPY(AttachmentBuffer);
    DEFAULT_MOVE(AttachmentBuffer);

    AttachmentBuffer(const AttachmentBufferSpecification& spec);

    ~AttachmentBuffer();

public:
    void free();

    constexpr VkImageView imageView() const { return VulkanObject<VkImageView>::m_handle; }

    constexpr VkFormat format() const { return m_format; }

    constexpr VkExtent2D extent() const { return m_extent; }

    constexpr VkSampleCountFlagBits sampleCount() const { return m_sampleCount; }

protected:
    VkDevice m_device = VK_NULL_HANDLE;

private:
    VkFormat m_format                   = VK_FORMAT_UNDEFINED;
    VkExtent2D m_extent                 = {};
    VkSampleCountFlagBits m_sampleCount = VK_SAMPLE_COUNT_1_BIT;
};

} // namespace R3::vulkan

#endif // R3_VULKAN

/**
 * @file vulkan-AttachmentBuffer.hxx
 * @copyright GNU Public License
 */

#pragma once

#if R3_VULKAN

#include "vulkan-fwd.hxx"
#include "vulkan-VulkanObject.hxx"
#include <api/Construct.hpp>
#include <api/Types.hpp>
#include <vulkan/vulkan.h>

namespace R3::vulkan {

/**
 * AttachmentBuffer Specification.
 */
struct AttachmentBufferSpecification {
    const PhysicalDevice& physicalDevice; /**< Valid PhysicalDevice. */
    const LogicalDevice& device;          /**< Valid LogicalDevice. */
    VkFormat format;                      /**< Image format. */
    VkExtent2D extent;                    /**< Image extent. */
    VkSampleCountFlagBits sampleCount;    /**< Image sample count. */
    VkImageAspectFlags aspectFlags;       /**< ImageView aspect flags. */
    VkImageUsageFlags imageUsage;         /**< Image usage. */
    VkMemoryPropertyFlags memoryProperty; /**< Memory allocation property. */
    uint32 mipLevels = 1;                 /**< Image miplevels. */
};

/**
 * AttachmentBuffer RAII wrapper.
 * Abstract class to be inherited by Attachment buffers i.e. color and depth buffers.
 * https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/VkImage.html
 * https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/VkImageView.html
 * https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/VkDeviceMemory.html
 */
class AttachmentBuffer : public VulkanObject<VkImage>,
                         public VulkanObject<VkImageView>,
                         public VulkanObject<VkDeviceMemory> {
public:
    using VulkanObject<VkImage>::vk; /**< Uses VkImage as main parent. */

protected:
    DEFAULT_CONSTRUCT(AttachmentBuffer);
    NO_COPY(AttachmentBuffer);
    DEFAULT_MOVE(AttachmentBuffer);

    /**
     * Construct AttachmentBuffer, allocating new Image.
     * @param spec
     */
    AttachmentBuffer(const AttachmentBufferSpecification& spec);

    /**
     * Destroy VkImage, VkImageView, and free VkDeviceMemory.
     */
    ~AttachmentBuffer();

public:
    /**
     * Free all resources, VkImage, VkImageView, and VkDeviceMemory.
     * Checks for validity of m_device, meaning it can be used when unitialized.
     */
    void free();

    /**
     * @return ImageView
     */
    constexpr VkImageView imageView() const { return VulkanObject<VkImageView>::vk(); }

protected:
    VkDevice m_device = VK_NULL_HANDLE; /**< Valid VkDevice */
};

} // namespace R3::vulkan

#endif // R3_VULKAN

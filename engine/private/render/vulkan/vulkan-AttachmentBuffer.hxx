////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file vulkan-AttachmentBuffer.hxx
/// @copyright GNU Public License
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#if R3_VULKAN

#include "vulkan-fwd.hxx"
#include "vulkan-VulkanObject.hxx"
#include <api/Construct.hpp>
#include <api/Result.hpp>
#include <vulkan/vulkan.h>

namespace R3::vulkan {

/// @brief AttachmentBuffer Specification.
/// The VkImageViewCreateInfo passed in WILL get mutated, assigning it a new image field
struct AttachmentBufferSpecification {
    const PhysicalDevice& physicalDevice;       ///< Valid PhysicalDevice.
    const LogicalDevice& device;                ///< Valid LogicalDevice.
    const VkImageCreateInfo& imageCreateInfo;   ///< Used for Image Creation.
    VkImageViewCreateInfo& imageViewCreateInfo; ///< Used for ImageView Creation, image field MUST be VK_NULL_HANDLE.
    VkMemoryPropertyFlags memoryPropertyFlags;  ///< Memory flags for allocation.
};

/// @brief AttachmentBuffer RAII wrapper.
/// Abstract class to be inherited by Attachment buffers i.e. color and depth buffers.
/// https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/VkImage.html
/// https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/VkImageView.html
/// https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/VkDeviceMemory.html
class AttachmentBuffer : public VulkanObject<VkImage>,
                         public VulkanObject<VkImageView>,
                         public VulkanObject<VkDeviceMemory> {
public:
    using VulkanObject<VkImage>::vk; ///< Uses VkImage as main parent.

protected:
    DEFAULT_CONSTRUCT(AttachmentBuffer);
    NO_COPY(AttachmentBuffer);
    DEFAULT_MOVE(AttachmentBuffer);

    /// @brief Initialize AttachmentBuffer, allocating new Image.
    /// Children of AttachmentBuffer MUST call initialize first in their create function.
    /// @param spec
    /// @return void | UnsupportedFeature | InitializationFailure | AllocationFailure
    Result<void> initialize(const AttachmentBufferSpecification& spec);

    /// @brief Destroy VkImage, VkImageView, and free VkDeviceMemory.
    ~AttachmentBuffer();

public:
    /// @brief Free all resources, VkImage, VkImageView, and VkDeviceMemory.
    /// Checks for validity of m_device, meaning it can be used when unitialized.
    void free();

    /// @return ImageView
    constexpr VkImageView imageView() const { return VulkanObject<VkImageView>::vk(); }

    /// @brief Check validity of internal handles.
    /// @return True if ll valid
    virtual constexpr bool valid() const override;

protected:
    VkDevice m_device = VK_NULL_HANDLE; ///< Valid VkDevice
};

} // namespace R3::vulkan

#endif // R3_VULKAN

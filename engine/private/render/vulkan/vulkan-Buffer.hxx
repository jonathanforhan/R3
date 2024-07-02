////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file vulkan-Buffer.hxx
/// @copyright GNU Public License
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#if R3_VULKAN

#include "vulkan-fwd.hxx"
#include "vulkan-VulkanObject.hxx"
#include <api/Construct.hpp>
#include <api/Result.hpp>
#include <tuple>
#include <vulkan/vulkan.h>

namespace R3::vulkan {

/// @brief Buffer Specification.
struct BufferSpecification {
    const LogicalDevice& device;
};

class Buffer : public VulkanObject<VkBuffer>, public VulkanObject<VkDeviceMemory> {
public:
    using VulkanObject<VkBuffer>::vk; ///< Uses VkBuffer as main parent.

protected:
    DEFAULT_CONSTRUCT(Buffer);
    NO_COPY(Buffer);
    DEFAULT_MOVE(Buffer);

    static Result<void> initialize(const BufferSpecification& spec);

    ~Buffer();

    static Result<std::tuple<VkBuffer, VkDeviceMemory>> allocate();

public:
    /// @brief Check validity of internal handles.
    /// @return True if ll valid
    virtual constexpr bool valid() const override;

protected:
    VkDevice m_device = VK_NULL_HANDLE; ///< Valid VkDevice
};

} // namespace R3::vulkan

#endif // R3_VULKAN

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file vulkan-Fence.hxx
/// @copyright GNU Public License
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#if R3_VULKAN

#include "vulkan-LogicalDevice.hxx"
#include "vulkan-VulkanObject.hxx"
#include <api/Construct.hpp>
#include <vulkan/vulkan.h>

namespace R3::vulkan {

/// @brief Vulkan Fence RAII wrapper.
class Fence : public VulkanObject<VkFence> {
public:
    DEFAULT_CONSTRUCT(Fence);
    NO_COPY(Fence);
    DEFAULT_MOVE(Fence);

    /// @brief Create a Vulkan Fence.
    /// @param device
    Fence(const LogicalDevice& device);

    /// @brief Destroy Semaphore
    ~Fence();

private:
    VkDevice m_device = VK_NULL_HANDLE;
};

inline Fence::Fence(const LogicalDevice& device)
    : m_device(device.vk()) {
    const VkFenceCreateInfo fenceCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
        .pNext = nullptr,
        .flags = VK_FENCE_CREATE_SIGNALED_BIT,
    };

    (void)vkCreateFence(m_device, &fenceCreateInfo, nullptr, &m_handle);
}

inline Fence::~Fence() {
    vkDestroyFence(m_device, m_handle, nullptr);
}

} // namespace R3::vulkan

#endif // R3_VULKAN

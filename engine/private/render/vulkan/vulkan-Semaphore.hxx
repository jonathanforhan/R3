////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file vulkan-Semaphore.hxx
/// @copyright GNU Public License
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#if R3_VULKAN

#include "vulkan-LogicalDevice.hxx"
#include "vulkan-VulkanObject.hxx"
#include <api/Construct.hpp>
#include <vulkan/vulkan.h>

namespace R3::vulkan {

/// @brief Vulkan Semaphore RAII wrapper.
class Semaphore : public VulkanObject<VkSemaphore> {
public:
    DEFAULT_CONSTRUCT(Semaphore);
    NO_COPY(Semaphore);
    DEFAULT_MOVE(Semaphore);

    /// @brief Create a Vulkan Semaphore.
    /// @param device
    Semaphore(const LogicalDevice& device);

    /// @brief Destroy Semaphore
    ~Semaphore();

private:
    VkDevice m_device = VK_NULL_HANDLE;
};

inline Semaphore::Semaphore(const LogicalDevice& device)
    : m_device(device.vk()) {
    const VkSemaphoreCreateInfo semaphoreCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
        .pNext = nullptr,
        .flags = {},
    };

    (void)vkCreateSemaphore(m_device, &semaphoreCreateInfo, nullptr, &m_handle);
}

inline Semaphore::~Semaphore() {
    vkDestroySemaphore(m_device, m_handle, nullptr);
}

} // namespace R3::vulkan

#endif // R3_VULKAN

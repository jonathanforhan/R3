#pragma once

#if R3_VULKAN

#include "vulkan-LogicalDevice.hxx"
#include "vulkan-VulkanObject.hxx"
#include <api/Assert.hpp>
#include <api/Construct.hpp>
#include <vulkan/vulkan.h>

namespace R3::vulkan {

class Semaphore : public VulkanObject<VkSemaphore> {
public:
    DEFAULT_CONSTRUCT(Semaphore);
    NO_COPY(Semaphore);
    DEFAULT_MOVE(Semaphore);

    Semaphore(const LogicalDevice& device);

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

    VkResult result = vkCreateSemaphore(m_device, &semaphoreCreateInfo, nullptr, &m_handle);
    ENSURE(result == VK_SUCCESS);
}

inline Semaphore::~Semaphore() {
    vkDestroySemaphore(m_device, m_handle, nullptr);
}

} // namespace R3::vulkan

#endif // R3_VULKAN

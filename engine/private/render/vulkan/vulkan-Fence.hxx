#pragma once

#if R3_VULKAN

#include "vulkan-LogicalDevice.hxx"
#include "vulkan-VulkanObject.hxx"
#include <api/Assert.hpp>
#include <api/Construct.hpp>
#include <vulkan/vulkan.h>

namespace R3::vulkan {

class Fence : public VulkanObject<VkFence> {
public:
    DEFAULT_CONSTRUCT(Fence);
    NO_COPY(Fence);
    DEFAULT_MOVE(Fence);

    Fence(const LogicalDevice& device);

    ~Fence();

    void reset();

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

    VkResult result = vkCreateFence(m_device, &fenceCreateInfo, nullptr, &m_handle);
    ENSURE(result == VK_SUCCESS);
}

inline Fence::~Fence() {
    vkDestroyFence(m_device, m_handle, nullptr);
}

inline void Fence::reset() {
    vkResetFences(m_device, 1, &m_handle);
}

} // namespace R3::vulkan

#endif // R3_VULKAN

#if R3_VULKAN

#include "vulkan-CommandPool.hxx"

#include "vulkan-LogicalDevice.hxx"
#include <api/Assert.hpp>
#include <api/Types.hpp>
#include <vulkan/vulkan.h>

namespace R3::vulkan {

R3::vulkan::CommandPool::CommandPool(const CommandPoolSpecification& spec)
    : m_device(spec.device.vk()) {
    const VkCommandPoolCreateInfo commandPoolCreateInfo = {
        .sType            = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .pNext            = nullptr,
        .flags            = spec.flags,
        .queueFamilyIndex = spec.queue.index(),
    };

    VkResult result = vkCreateCommandPool(spec.device.vk(), &commandPoolCreateInfo, nullptr, &m_handle);
    ENSURE(result == VK_SUCCESS);

    const VkCommandBufferAllocateInfo commandBufferAllocateInfo = {
        .sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .pNext              = nullptr,
        .commandPool        = m_handle,
        .level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = spec.commandBufferCount,
    };

    m_commandBuffers.resize(spec.commandBufferCount);
    result = vkAllocateCommandBuffers(m_device, &commandBufferAllocateInfo, m_commandBuffers.data());
    ENSURE(result == VK_SUCCESS);
}

CommandPool::~CommandPool() {
    if (m_handle) {
        // vkFreeCommandBuffers(m_device, m_handle, static_cast<uint32>(m_commandBuffers.size()),
        // m_commandBuffers.data());
        vkDestroyCommandPool(m_device, m_handle, nullptr);
    }
}

} // namespace R3::vulkan

#endif // R3_VULKAN

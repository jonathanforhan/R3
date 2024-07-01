#if R3_VULKAN

#include "vulkan-CommandPool.hxx"

#include "vulkan-LogicalDevice.hxx"
#include <api/Log.hpp>
#include <expected>
#include <vulkan/vulkan.h>

namespace R3::vulkan {

Result<CommandPool> CommandPool::create(const CommandPoolSpecification& spec) {
    CommandPool self;
    self.m_device = spec.device.vk();

    const VkCommandPoolCreateInfo commandPoolCreateInfo = {
        .sType            = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .pNext            = nullptr,
        .flags            = spec.flags,
        .queueFamilyIndex = spec.queue.index(),
    };

    VkResult result = vkCreateCommandPool(spec.device.vk(), &commandPoolCreateInfo, nullptr, &self.m_handle);
    if (result != VK_SUCCESS) {
        R3_LOG(Error, "vkCreateCommandPool failure {}", (int)result);
        return std::unexpected(Error::InitializationFailure);
    }

    const VkCommandBufferAllocateInfo commandBufferAllocateInfo = {
        .sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .pNext              = nullptr,
        .commandPool        = self.m_handle,
        .level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = spec.commandBufferCount,
    };

    self.m_commandBuffers.resize(spec.commandBufferCount);
    result = vkAllocateCommandBuffers(self.m_device, &commandBufferAllocateInfo, self.m_commandBuffers.data());
    if (result != VK_SUCCESS) {
        R3_LOG(Error, "vkAllocateCommandBuffers failure {}", (int)result);
        return std::unexpected(Error::AllocationFailure);
    }

    return self;
}

CommandPool::~CommandPool() {
    vkDestroyCommandPool(m_device, m_handle, nullptr);
}

} // namespace R3::vulkan

#endif // R3_VULKAN

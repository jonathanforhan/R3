#if R3_VULKAN

#include "render/CommandPool.hpp"

#include <vulkan/vulkan.h>
#include "api/Check.hpp"
#include "api/Ensure.hpp"
#include "render/LogicalDevice.hpp"

namespace R3 {

void CommandPool::create(const CommandPoolSpecification& spec) {
    CHECK(spec.logicalDevice != nullptr);
    CHECK(spec.swapchain != nullptr);
    CHECK(spec.commandBufferCount > 0);
    m_spec = spec;

    VkCommandPoolCreateInfo commandPoolCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .pNext = nullptr,
        .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
        .queueFamilyIndex = m_spec.logicalDevice->graphicsQueue().index(),
    };

    ENSURE(vkCreateCommandPool(m_spec.logicalDevice->handle<VkDevice>(),
                               &commandPoolCreateInfo,
                               nullptr,
                               handlePtr<VkCommandPool*>()) == VK_SUCCESS);

    m_commandBuffers.resize(m_spec.commandBufferCount);
    for (auto& commandBuffer : m_commandBuffers) {
        commandBuffer.create({
            .logicalDevice = m_spec.logicalDevice,
            .swapchain = m_spec.swapchain,
            .commandPool = this,
        });
    }
}

void CommandPool::destroy() {
    for (auto& commandBuffer : m_commandBuffers) {
        commandBuffer.destroy();
    }
    vkDestroyCommandPool(m_spec.logicalDevice->handle<VkDevice>(), handle<VkCommandPool>(), nullptr);
}

} // namespace R3

#endif // R3_VULKAN
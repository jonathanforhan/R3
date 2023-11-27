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

    m_commandBuffer.create({
        .logicalDevice = m_spec.logicalDevice,
        .swapchain = m_spec.swapchain,
        .commandPool = this,
    });
}

void CommandPool::destroy() {
    m_commandBuffer.destroy();
    vkDestroyCommandPool(m_spec.logicalDevice->handle<VkDevice>(), handle<VkCommandPool>(), nullptr);
}

} // namespace R3

#endif // R3_VULKAN
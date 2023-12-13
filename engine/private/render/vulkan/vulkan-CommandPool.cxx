#if R3_VULKAN

#include "render/CommandPool.hpp"

#include <vulkan/vulkan.h>
#include "api/Check.hpp"
#include "api/Ensure.hpp"
#include "render/LogicalDevice.hpp"

namespace R3 {

namespace local {

static constexpr VkCommandPoolCreateFlags CommandPoolFlagsToVkFlags(CommandPoolFlags flags) {
    switch (flags) {
        case CommandPoolFlags::Protected:
            return VK_COMMAND_POOL_CREATE_PROTECTED_BIT;
        case CommandPoolFlags::Reset:
            return VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        case CommandPoolFlags::Transient:
            return VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
        default:
            ENSURE(false);
    }
}

} // namespace local

CommandPool::CommandPool(const CommandPoolSpecification& spec)
    : m_spec(spec) {

    VkCommandPoolCreateInfo commandPoolCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .pNext = nullptr,
        .flags = local::CommandPoolFlagsToVkFlags(m_spec.flags),
        .queueFamilyIndex = m_spec.logicalDevice->graphicsQueue().index(),
    };

    VkCommandPool tmp;
    VkResult result = vkCreateCommandPool(m_spec.logicalDevice->as<VkDevice>(), &commandPoolCreateInfo, nullptr, &tmp);
    ENSURE(result == VK_SUCCESS);
    setHandle(tmp);

    m_commandBuffers = CommandBuffer::allocate({
        .logicalDevice = m_spec.logicalDevice,
        .swapchain = m_spec.swapchain,
        .commandPool = this,
        .commandBufferCount = m_spec.commandBufferCount,
    });
}

CommandPool::~CommandPool() {
    if (validHandle()) {
        vkDestroyCommandPool(m_spec.logicalDevice->as<VkDevice>(), as<VkCommandPool>(), nullptr);
    }
}

} // namespace R3

#endif // R3_VULKAN
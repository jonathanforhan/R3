#if R3_VULKAN

#include "render/CommandPool.hxx"

#include <vulkan/vulkan.hpp>
#include "api/Ensure.hpp"
#include "render/LogicalDevice.hxx"

namespace R3 {

namespace local {

static constexpr vk::CommandPoolCreateFlags CommandPoolFlagsToVkFlags(CommandPoolType flags) {
    switch (flags) {
        case CommandPoolType::Protected:
            return vk::CommandPoolCreateFlagBits::eProtected;
        case CommandPoolType::Reset:
            return vk::CommandPoolCreateFlagBits::eResetCommandBuffer;
        case CommandPoolType::Transient:
            return vk::CommandPoolCreateFlagBits::eTransient;
        default:
            ENSURE(false);
    }
}

} // namespace local

CommandPool::CommandPool(const CommandPoolSpecification& spec)
    : m_logicalDevice(&spec.logicalDevice) {
    const vk::CommandPoolCreateInfo commandPoolCreateInfo = {
        .sType = vk::StructureType::eCommandPoolCreateInfo,
        .pNext = nullptr,
        .flags = local::CommandPoolFlagsToVkFlags(spec.type),
        .queueFamilyIndex = m_logicalDevice->graphicsQueue().index(),
    };

    setHandle(m_logicalDevice->as<vk::Device>().createCommandPool(commandPoolCreateInfo));

    m_commandBuffers = CommandBuffer::allocate({
        .logicalDevice = *m_logicalDevice,
        .swapchain = spec.swapchain,
        .commandPool = *this,
        .commandBufferCount = spec.commandBufferCount,
    });
}

CommandPool::~CommandPool() {
    if (validHandle()) {
        m_logicalDevice->as<vk::Device>().destroyCommandPool(as<vk::CommandPool>());
    }
}

} // namespace R3

#endif // R3_VULKAN
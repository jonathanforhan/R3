#if R3_VULKAN

#include "render/CommandPool.hpp"

#include <vulkan/vulkan.hpp>
#include "api/Check.hpp"
#include "api/Ensure.hpp"
#include "api/Log.hpp"
#include "render/LogicalDevice.hpp"

namespace R3 {

namespace local {

static constexpr vk::CommandPoolCreateFlags CommandPoolFlagsToVkFlags(CommandPoolFlags flags) {
    switch (flags) {
        case CommandPoolFlags::Protected:
            return vk::CommandPoolCreateFlagBits::eProtected;
        case CommandPoolFlags::Reset:
            return vk::CommandPoolCreateFlagBits::eResetCommandBuffer;
        case CommandPoolFlags::Transient:
            return vk::CommandPoolCreateFlagBits::eTransient;
        default:
            ENSURE(false);
    }
}

} // namespace local

CommandPool::CommandPool(const CommandPoolSpecification& spec)
    : m_spec(spec) {

    vk::CommandPoolCreateInfo commandPoolCreateInfo = {
        .sType = vk::StructureType::eCommandPoolCreateInfo,
        .pNext = nullptr,
        .flags = local::CommandPoolFlagsToVkFlags(m_spec.flags),
        .queueFamilyIndex = m_spec.logicalDevice->graphicsQueue().index(),
    };

    setHandle(m_spec.logicalDevice->as<vk::Device>().createCommandPool(commandPoolCreateInfo));

    m_commandBuffers = CommandBuffer::allocate({
        .logicalDevice = m_spec.logicalDevice,
        .swapchain = m_spec.swapchain,
        .commandPool = this,
        .commandBufferCount = m_spec.commandBufferCount,
    });
}

CommandPool::~CommandPool() {
    if (validHandle()) {
        m_spec.logicalDevice->as<vk::Device>().destroyCommandPool(as<vk::CommandPool>());
    }
}

} // namespace R3

#endif // R3_VULKAN
#if R3_VULKAN

#include "render/CommandPool.hpp"

#include <vulkan/vulkan.hpp>
#include "api/Check.hpp"
#include "api/Ensure.hpp"
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

void CommandPool::create(const CommandPoolSpecification& spec) {
    CHECK(spec.logicalDevice != nullptr);
    CHECK(spec.swapchain != nullptr);
    m_spec = spec;

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

void CommandPool::destroy() {
    CommandBuffer::free(m_commandBuffers);
    m_spec.logicalDevice->as<vk::Device>().destroyCommandPool(as<vk::CommandPool>());
}

} // namespace R3

#endif // R3_VULKAN
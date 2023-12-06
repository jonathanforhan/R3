#if R3_VULKAN

#include "render/CommandPool.hpp"

#include <vulkan/vulkan.hpp>
#include "api/Check.hpp"
#include "render/LogicalDevice.hpp"

namespace R3 {

void CommandPool::create(const CommandPoolSpecification& spec) {
    CHECK(spec.logicalDevice != nullptr);
    CHECK(spec.swapchain != nullptr);
    CHECK(spec.commandBufferCount > 0);
    m_spec = spec;

    vk::CommandPoolCreateInfo commandPoolCreateInfo = {
        .sType = vk::StructureType::eCommandPoolCreateInfo,
        .pNext = nullptr,
        .flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
        .queueFamilyIndex = m_spec.logicalDevice->graphicsQueue().index(),
    };

    setHandle(m_spec.logicalDevice->as<vk::Device>().createCommandPool(commandPoolCreateInfo));

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
    m_spec.logicalDevice->as<vk::Device>().destroyCommandPool(as<vk::CommandPool>());
}

} // namespace R3

#endif // R3_VULKAN
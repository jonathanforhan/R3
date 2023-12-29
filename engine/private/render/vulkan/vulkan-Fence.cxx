#if R3_VULKAN

#include "render/Fence.hxx"

#include <vulkan/vulkan.hpp>
#include "render/LogicalDevice.hxx"

namespace R3 {

Fence::Fence(const FenceSpecification& spec)
    : m_logicalDevice(&spec.logicalDevice) {
    const vk::FenceCreateInfo fenceCreateInfo = {
        .sType = vk::StructureType::eFenceCreateInfo,
        .pNext = nullptr,
        .flags = vk::FenceCreateFlagBits::eSignaled,
    };

    setHandle(m_logicalDevice->as<vk::Device>().createFence(fenceCreateInfo));
}

Fence::~Fence() {
    if (validHandle()) {
        m_logicalDevice->as<vk::Device>().destroyFence(as<vk::Fence>());
    }
}

void Fence::reset() {
    m_logicalDevice->as<vk::Device>().resetFences(as<vk::Fence>());
}

} // namespace R3

#endif // R3_VULKAN

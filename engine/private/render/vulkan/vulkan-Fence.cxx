#if R3_VULKAN

#include "render/Fence.hpp"

#include <vulkan/vulkan.hpp>
#include "api/Check.hpp"
#include "render/LogicalDevice.hpp"

namespace R3 {

Fence::Fence(const FenceSpecification& spec)
    : m_spec(spec) {

    vk::FenceCreateInfo fenceCreateInfo = {
        .sType = vk::StructureType::eFenceCreateInfo,
        .pNext = nullptr,
        .flags = vk::FenceCreateFlagBits::eSignaled,
    };

    setHandle(m_spec.logicalDevice->as<vk::Device>().createFence(fenceCreateInfo));
}

Fence::~Fence() {
    if (validHandle()) {
        m_spec.logicalDevice->as<vk::Device>().destroyFence(as<vk::Fence>());
    }
}

} // namespace R3

#endif // R3_VULKAN

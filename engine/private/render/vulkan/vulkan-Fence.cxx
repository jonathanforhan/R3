#if R3_VULKAN

#include "render/Fence.hpp"

#include <vulkan/vulkan.hpp>
#include "api/Check.hpp"
#include "render/LogicalDevice.hpp"

namespace R3 {

void Fence::create(const FenceSpecification& spec) {
    CHECK(spec.logicalDevice != nullptr);
    m_spec = spec;

    vk::FenceCreateInfo fenceCreateInfo = {
        .sType = vk::StructureType::eFenceCreateInfo,
        .pNext = nullptr,
        .flags = vk::FenceCreateFlagBits::eSignaled,
    };

    setHandle(m_spec.logicalDevice->as<vk::Device>().createFence(fenceCreateInfo));
}

void Fence::destroy() {
    m_spec.logicalDevice->as<vk::Device>().destroyFence(as<vk::Fence>());
}

} // namespace R3

#endif // R3_VULKAN

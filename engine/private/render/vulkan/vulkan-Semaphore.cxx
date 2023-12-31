#if R3_VULKAN

#include "render/Semaphore.hpp"

#include <vulkan/vulkan.hpp>
#include "render/LogicalDevice.hpp"

namespace R3 {

Semaphore::Semaphore(const SemaphoreSpecification& spec)
    : m_logicalDevice(&spec.logicalDevice) {
    const vk::SemaphoreCreateInfo semaphoreCreateInfo = {
        .sType = vk::StructureType::eSemaphoreCreateInfo,
        .pNext = nullptr,
        .flags = {},
    };

    setHandle(m_logicalDevice->as<vk::Device>().createSemaphore(semaphoreCreateInfo));
}

Semaphore::~Semaphore() {
    if (validHandle()) {
        m_logicalDevice->as<vk::Device>().destroySemaphore(as<vk::Semaphore>());
    }
}

} // namespace R3

#endif // R3_VULKAN
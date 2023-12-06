#if R3_VULKAN

#include "render/Semaphore.hpp"

#include <vulkan/vulkan.hpp>
#include "api/Check.hpp"
#include "render/LogicalDevice.hpp"

namespace R3 {

void Semaphore::create(const SemaphoreSpecification& spec) {
    CHECK(spec.logicalDevice != nullptr);
    m_spec = spec;

    vk::SemaphoreCreateInfo semaphoreCreateInfo = {
        .sType = vk::StructureType::eSemaphoreCreateInfo,
        .pNext = nullptr,
        .flags = {},
    };

    setHandle(m_spec.logicalDevice->as<vk::Device>().createSemaphore(semaphoreCreateInfo));
}

void Semaphore::destroy() {
    m_spec.logicalDevice->as<vk::Device>().destroySemaphore(as<vk::Semaphore>());
}

} // namespace R3

#endif // R3_VULKAN
#if R3_VULKAN

#include "render/Semaphore.hpp"

#include <vulkan/vulkan.h>
#include "api/Check.hpp"
#include "api/Ensure.hpp"
#include "render/LogicalDevice.hpp"

namespace R3 {

Semaphore::Semaphore(const SemaphoreSpecification& spec)
    : m_spec(spec) {
    const VkSemaphoreCreateInfo semaphoreCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
        .pNext = nullptr,
        .flags = {},
    };

    VkSemaphore tmp;
    VkResult result = vkCreateSemaphore(m_spec.logicalDevice->as<VkDevice>(), &semaphoreCreateInfo, nullptr, &tmp);
    ENSURE(result == VK_SUCCESS);
    setHandle(tmp);
}

Semaphore::~Semaphore() {
    if (validHandle()) {
        vkDestroySemaphore(m_spec.logicalDevice->as<VkDevice>(), as<VkSemaphore>(), nullptr);
    }
}

} // namespace R3

#endif // R3_VULKAN
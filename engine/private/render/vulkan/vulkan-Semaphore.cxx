#if R3_VULKAN

#include "render/Semaphore.hpp"

#include <vulkan/vulkan.h>
#include "api/Check.hpp"
#include "api/Ensure.hpp"
#include "render/LogicalDevice.hpp"

namespace R3 {

void Semaphore::create(const SemaphoreSpecification& spec) {
    CHECK(spec.logicalDevice != nullptr);
    m_spec = spec;

    VkSemaphoreCreateInfo semaphoreCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0U,
    };
    ENSURE(vkCreateSemaphore(
               m_spec.logicalDevice->handle<VkDevice>(), &semaphoreCreateInfo, nullptr, handlePtr<VkSemaphore*>()) ==
           VK_SUCCESS);
}

void Semaphore::destroy() {
    vkDestroySemaphore(m_spec.logicalDevice->handle<VkDevice>(), handle<VkSemaphore>(), nullptr);
}

} // namespace R3

#endif // R3_VULKAN
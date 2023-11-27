#if R3_VULKAN

#include "render/Fence.hpp"

#include <vulkan/vulkan.h>
#include "api/Check.hpp"
#include "api/Ensure.hpp"
#include "render/LogicalDevice.hpp"

namespace R3 {

void Fence::create(const FenceSpecification& spec) {
    CHECK(spec.logicalDevice != nullptr);
    m_spec = spec;

    VkFenceCreateInfo fenceCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
        .pNext = nullptr,
        .flags = VK_FENCE_CREATE_SIGNALED_BIT,
    };

    ENSURE(vkCreateFence(m_spec.logicalDevice->handle<VkDevice>(), &fenceCreateInfo, nullptr, handlePtr<VkFence*>()) ==
           VK_SUCCESS);
}

void Fence::destroy() {
    vkDestroyFence(m_spec.logicalDevice->handle<VkDevice>(), handle<VkFence>(), nullptr);
}

} // namespace R3

#endif // R3_VULKAN

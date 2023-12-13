#if R3_VULKAN

#include "render/Fence.hpp"

#include <vulkan/vulkan.h>
#include "api/Check.hpp"
#include "api/Ensure.hpp"
#include "render/LogicalDevice.hpp"

namespace R3 {

Fence::Fence(const FenceSpecification& spec)
    : m_spec(spec) {

    const VkFenceCreateInfo fenceCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
        .pNext = nullptr,
        .flags = VK_FENCE_CREATE_SIGNALED_BIT,
    };

    VkFence tmp;
    VkResult result = vkCreateFence(m_spec.logicalDevice->as<VkDevice>(), &fenceCreateInfo, nullptr, &tmp);
    ENSURE(result == VK_SUCCESS);
    setHandle(tmp);
}

Fence::~Fence() {
    if (validHandle()) {
        vkDestroyFence(m_spec.logicalDevice->as<VkDevice>(), as<VkFence>(), nullptr);
    }
}

} // namespace R3

#endif // R3_VULKAN

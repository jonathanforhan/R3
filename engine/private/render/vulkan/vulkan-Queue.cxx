#if R3_VULKAN

#include "vulkan-Queue.hxx"

#include "vulkan-LogicalDevice.hxx"
#include "vulkan-PhysicalDevice.hxx"
#include "vulkan-Surface.hxx"
#include <api/Log.hpp>
#include <api/Result.hpp>
#include <api/Types.hpp>
#include <cassert>
#include <expected>
#include <vector>
#include <vulkan/vulkan.h>

namespace R3::vulkan {

Result<QueueFamilyIndices> QueueFamilyIndices::query(const PhysicalDevice& physicalDevice, const Surface& surface) {
    return query(physicalDevice.vk(), surface.vk());
}

Result<QueueFamilyIndices> QueueFamilyIndices::query(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface) {
    uint32 queueFamilyCount;
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilies.data());

    QueueFamilyIndices queueFamilyIndices;

    for (uint32 i = 0; const auto& queueFamily : queueFamilies) {
        VkBool32 presentationSupport;
        (void)vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, surface, &presentationSupport);

        if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            queueFamilyIndices.graphics = i;
        }

        if (presentationSupport) {
            queueFamilyIndices.presentation = i;
        }

        if (queueFamilyIndices.valid()) {
            return queueFamilyIndices;
        }

        i++;
    }

    R3_LOG(Error, "invalid queue family indices");
    return std::unexpected(Error::InitializationFailure);
}

void Queue::acquire(const QueueSpecification& spec) {
    m_queueType  = spec.queueType;
    m_queueIndex = spec.queueIndex;
    vkGetDeviceQueue(spec.device.vk(), m_queueIndex, 0, &m_handle);
}

} // namespace R3::vulkan

#endif // R3_VULKAN

#if R3_VULKAN

#include "vulkan-Queue.hxx"

#include "vulkan-LogicalDevice.hxx"
#include <api/Types.hpp>
#include <vector>
#include <vulkan/vulkan.h>

namespace R3::vulkan {

QueueFamilyIndices QueueFamilyIndices::query(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface) {
    uint32 queueFamilyCount;
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilies.data());

    QueueFamilyIndices queueFamilyIndices;

    for (uint32 i = 0; const auto& queueFamily : queueFamilies) {
        VkBool32 presentationSupport;
        vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, surface, &presentationSupport);

        if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            queueFamilyIndices.graphics = i;
        }

        if (presentationSupport) {
            queueFamilyIndices.presentation = i;
        }

        if (queueFamilyIndices.isValid()) {
            break;
        }

        i++;
    }

    return queueFamilyIndices;
}

void Queue::acquire(const QueueSpecification& spec) {
    m_queueType  = spec.queueType;
    m_queueIndex = spec.queueIndex;
    vkGetDeviceQueue(spec.device.vk(), m_queueIndex, 0, &m_handle);
}

} // namespace R3::vulkan

#endif // R3_VULKAN

#if R3_VULKAN

#include "render/Queues.hpp"

#include <vulkan/vulkan.h>
#include <vector>
#include "api/Check.hpp"

namespace R3 {

QueueFamilyIndices Queues::queryQueueFamilyIndices(Handle physicalDeviceHandle) {
    CHECK(physicalDeviceHandle != nullptr);
    VkPhysicalDevice vkPhysicalDevice = (VkPhysicalDevice)physicalDeviceHandle;

    QueueFamilyIndices queueFamilyIndices;

    uint32 queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(vkPhysicalDevice, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(vkPhysicalDevice, &queueFamilyCount, queueFamilies.data());

    for (uint32 i = 0; const auto& queueFamily : queueFamilies) {
        if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            queueFamilyIndices.graphics = i;
        }

        if (queueFamilyIndices.isValid()) {
            break;
        }

        i++;
    }

    return queueFamilyIndices;
}

} // namespace R3

#endif // R3_VULKAN

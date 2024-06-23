#if R3_VULKAN

#include "vulkan-Queue.hxx"

#include "vulkan-LogicalDevice.hxx"
#include <api/Types.hpp>
#include <mutex>
#include <vector>
#include <vulkan/vulkan.h>

namespace R3::vulkan {

static std::mutex s_graphicsMutex;
static std::mutex s_presentationMutex;
static std::mutex s_computeMutex;

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

    switch (m_queueType) {
        case QueueType::Graphics:
            m_mutex = &s_graphicsMutex;
            break;
        case QueueType::Presentation:
            m_mutex = &s_presentationMutex;
            break;
        case QueueType::Compute:
            m_mutex = &s_computeMutex;
            break;
    }
}

} // namespace R3::vulkan

#endif // R3_VULKAN

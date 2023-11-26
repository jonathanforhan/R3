#if R3_VULKAN

#include "render/Queue.hpp"

#include <vulkan/vulkan.h>
#include <vector>
#include "api/Check.hpp"
#include "render/LogicalDevice.hpp"

namespace R3 {

QueueFamilyIndices QueueFamilyIndices::query(NativeRenderObject::Handle physicalDeviceHandle,
                                             NativeRenderObject::Handle surfaceHandle) {
    CHECK(physicalDeviceHandle != nullptr);
    CHECK(surfaceHandle != nullptr);
    VkPhysicalDevice vkPhysicalDevice = (VkPhysicalDevice)physicalDeviceHandle;
    VkSurfaceKHR vkSurface = (VkSurfaceKHR)surfaceHandle;

    QueueFamilyIndices queueFamilyIndices;

    uint32 queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(vkPhysicalDevice, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(vkPhysicalDevice, &queueFamilyCount, queueFamilies.data());

    for (uint32 i = 0; const auto& queueFamily : queueFamilies) {
        VkBool32 presentationSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(vkPhysicalDevice, i, vkSurface, &presentationSupport);

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
    CHECK(spec.logicalDevice != nullptr);
    m_spec = spec;

    vkGetDeviceQueue(m_spec.logicalDevice->handle<VkDevice>(), spec.queueIndex, 0, handlePtr<VkQueue*>());
}

} // namespace R3

#endif // R3_VULKAN

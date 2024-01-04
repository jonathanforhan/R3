#if R3_VULKAN

#include "render/Queue.hpp"

#include <vulkan/vulkan.hpp>
#include "api/Check.hpp"
#include "render/LogicalDevice.hpp"

namespace R3 {

QueueFamilyIndices QueueFamilyIndices::query(NativeRenderObject&& physicalDeviceHandle,
                                             NativeRenderObject&& surfaceHandle) {
    const vk::PhysicalDevice physicalDevice = physicalDeviceHandle.as<vk::PhysicalDevice>();
    const vk::SurfaceKHR surface = surfaceHandle.as<vk::SurfaceKHR>();

    const auto queueFamilies = physicalDevice.getQueueFamilyProperties();
    QueueFamilyIndices queueFamilyIndices;

    for (uint32 i = 0; const auto& queueFamily : queueFamilies) {
        vk::Bool32 presentationSupport = physicalDevice.getSurfaceSupportKHR(i, surface);

        if (queueFamily.queueFlags & vk::QueueFlagBits::eGraphics) {
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
    m_queueType = spec.queueType;
    m_queueIndex = spec.queueIndex;
    setHandle(spec.logicalDevice->as<vk::Device>().getQueue(m_queueIndex, 0));
}

} // namespace R3

#endif // R3_VULKAN

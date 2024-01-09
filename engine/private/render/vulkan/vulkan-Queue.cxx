#if R3_VULKAN

#include "render/Queue.hpp"

#include <mutex>
#include <vulkan/vulkan.hpp>
#include "render/LogicalDevice.hpp"

namespace R3 {

static std::mutex s_graphicsMutex;
static std::mutex s_presentationMutex;

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

std::unique_lock<std::mutex> Queue::lock() const {
    if (m_queueType == QueueType::Graphics) {
        return std::unique_lock(s_graphicsMutex);
    } else {
        return std::unique_lock(s_presentationMutex);
    }
}

} // namespace R3

#endif // R3_VULKAN

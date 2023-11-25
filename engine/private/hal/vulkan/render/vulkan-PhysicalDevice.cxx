#if R3_VULKAN

#include "render/PhysicalDevice.hpp"

#include <vulkan/vulkan.h>
#include "api/Check.hpp"
#include "api/Ensure.hpp"
#include "api/Log.hpp"
#include "render/Queues.hpp"

namespace R3 {

void PhysicalDevice::select(const PhysicalDeviceSpecification& spec) {
    CHECK(spec.instance != nullptr);
    m_spec = spec;

    uint32 physicalDeviceCount = 0;
    vkEnumeratePhysicalDevices(m_spec.instance->handle<VkInstance>(), &physicalDeviceCount, nullptr);
    ENSURE(physicalDeviceCount != 0);

    std::vector<VkPhysicalDevice> physicalDevices(physicalDeviceCount);
    vkEnumeratePhysicalDevices(m_spec.instance->handle<VkInstance>(), &physicalDeviceCount, physicalDevices.data());

    int32 bestScore = INT32_MIN;
    for (VkPhysicalDevice physicalDevice : physicalDevices) {
        int32 score = evaluateDevice(physicalDevice);
        if (score > bestScore) {
            setHandle(physicalDevice);
            bestScore = score;
        }
    }

    ENSURE(handle() != nullptr && bestScore >= 0);
}

int32 PhysicalDevice::evaluateDevice(Handle deviceHandle) const {
    VkPhysicalDevice physicalDevice = (VkPhysicalDevice)deviceHandle;

    VkPhysicalDeviceProperties physicalDeviceProperties;
    vkGetPhysicalDeviceProperties(physicalDevice, &physicalDeviceProperties);

    VkPhysicalDeviceFeatures physicalDeviceFeatures;
    vkGetPhysicalDeviceFeatures(physicalDevice, &physicalDeviceFeatures);

    int32 deviceScore = 0;

    if (physicalDeviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
        deviceScore += 100;
    }

    if (!Queues::queryQueueFamilyIndices((VkPhysicalDevice)deviceHandle).isValid()) {
        deviceScore -= 10000;
    }

    return deviceScore;
}

}

#endif // R3_VULKAN

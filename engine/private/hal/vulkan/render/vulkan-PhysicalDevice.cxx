#if R3_VULKAN

#include "render/PhysicalDevice.hpp"

#include <vulkan/vulkan.h>
#include "api/Check.hpp"
#include "api/Ensure.hpp"
#include "api/Log.hpp"
#include "render/Instance.hpp"
#include "render/Queue.hpp"
#include "render/Surface.hpp"
#include "vulkan-SwapchainSupportDetails.hxx"

namespace R3 {

void PhysicalDevice::select(const PhysicalDeviceSpecification& spec) {
    CHECK(spec.instance != nullptr);
    CHECK(spec.surface != nullptr);
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
    VkSurfaceKHR surface = m_spec.surface->handle<VkSurfaceKHR>();

    VkPhysicalDeviceProperties physicalDeviceProperties;
    vkGetPhysicalDeviceProperties(physicalDevice, &physicalDeviceProperties);

    VkPhysicalDeviceFeatures physicalDeviceFeatures;
    vkGetPhysicalDeviceFeatures(physicalDevice, &physicalDeviceFeatures);

    int32 deviceScore = 0;

    auto deviceQueueIndices = QueueFamilyIndices::query(physicalDevice, surface);
    auto swapchainSupportDetails = vulkan::SwapchainSupportDetails::query(physicalDevice, surface);

    if (!deviceQueueIndices.isValid())
        deviceScore -= 10000;

    if (!checkExtensionSupport(deviceHandle))
        deviceScore -= 10000;

    if (!swapchainSupportDetails.isValid())
        deviceScore -= 10000;

    if (deviceQueueIndices.presentation != deviceQueueIndices.graphics)
        deviceScore -= 10;

    if (physicalDeviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
        deviceScore += 100;

    return deviceScore;
}

bool PhysicalDevice::checkExtensionSupport(Handle deviceHandle) const {
    uint32 deviceExtensionCount = 0;
    vkEnumerateDeviceExtensionProperties((VkPhysicalDevice)deviceHandle, nullptr, &deviceExtensionCount, nullptr);
    CHECK(deviceExtensionCount != 0);

    std::vector<VkExtensionProperties> deviceExtensions(deviceExtensionCount);
    vkEnumerateDeviceExtensionProperties(
        (VkPhysicalDevice)deviceHandle, nullptr, &deviceExtensionCount, deviceExtensions.data());

    for (const auto& requiredExtension : extensions()) {
        auto it = std::ranges::find_if(deviceExtensions, [=](VkExtensionProperties& extension) -> bool {
            return strcmp(requiredExtension, extension.extensionName) == 0;
        });

        if (it == deviceExtensions.end()) {
            return false;
        }
    }

    return true;
}

} // namespace R3

#endif // R3_VULKAN

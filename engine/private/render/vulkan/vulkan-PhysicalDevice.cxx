#if R3_VULKAN

#include "render/PhysicalDevice.hpp"

#include <vulkan/vulkan.hpp>
#include <cstring>
#include "api/Check.hpp"
#include "api/Ensure.hpp"
#include "api/Log.hpp"
#include "render/Instance.hpp"
#include "render/Queue.hpp"
#include "render/Surface.hpp"
#include "vulkan-SwapchainSupportDetails.hxx"

namespace R3 {

PhysicalDevice::PhysicalDevice(const PhysicalDeviceSpecification& spec)
    : m_spec(spec) {
    std::vector<vk::PhysicalDevice> physicalDevices = m_spec.instance->as<vk::Instance>().enumeratePhysicalDevices();

    int32 bestScore = INT32_MIN;
    for (vk::PhysicalDevice physicalDevice : physicalDevices) {
        int32 score = evaluateDevice(physicalDevice);
        if (score > bestScore) {
            setHandle(physicalDevice);
            bestScore = score;
        }
    }

    ENSURE(validHandle() && bestScore >= 0);
}

int32 PhysicalDevice::evaluateDevice(const NativeRenderObject& deviceHandle) const {
    vk::PhysicalDevice physicalDevice = deviceHandle.as<vk::PhysicalDevice>();
    auto surface = m_spec.surface->as<vk::SurfaceKHR>();

    auto physicalDeviceProperties = physicalDevice.getProperties();
    auto physicalDeviceFeatures = physicalDevice.getFeatures();

    int32 deviceScore = 0;

    auto deviceQueueIndices = QueueFamilyIndices::query(physicalDevice, surface);
    auto swapchainSupportDetails = vulkan::SwapchainSupportDetails::query(physicalDevice, surface);

    if (!deviceQueueIndices.isValid())
        deviceScore -= 1000;

    if (!checkExtensionSupport(deviceHandle))
        deviceScore -= 1000;

    if (!swapchainSupportDetails.isValid())
        deviceScore -= 1000;

    if (!physicalDeviceFeatures.samplerAnisotropy)
        deviceScore -= 1000;

    if (deviceQueueIndices.presentation != deviceQueueIndices.graphics)
        deviceScore -= 10;

    if (physicalDeviceProperties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu)
        deviceScore += 100;

    return deviceScore;
}

uint32 PhysicalDevice::queryMemoryType(uint32 typeFilter, uint64 propertyFlags) const {
    vk::PhysicalDeviceMemoryProperties memoryProperties = as<vk::PhysicalDevice>().getMemoryProperties();
    vk::MemoryPropertyFlags flags((uint32)propertyFlags);

    for (uint32 i = 0; i < memoryProperties.memoryTypeCount; i++) {
        if ((typeFilter & (1 << i)) && (memoryProperties.memoryTypes[i].propertyFlags & flags) == flags)
            return i;
    }

    ENSURE(false); /* unable to find suitable memory type */
}

bool PhysicalDevice::checkExtensionSupport(const NativeRenderObject& deviceHandle) const {
    auto deviceExtensions = deviceHandle.as<vk::PhysicalDevice>().enumerateDeviceExtensionProperties();

    for (const auto* requiredExtension : extensions()) {
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

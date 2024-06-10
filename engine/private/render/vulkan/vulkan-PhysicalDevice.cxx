#include "vulkan-PhysicalDevice.hxx"

#include "api/Assert.hpp"
#include "api/Types.hpp"
#include "vulkan-Instance.hxx"
#include "vulkan-Queue.hxx"
#include "vulkan-Surface.hxx"
#include "vulkan-Swapchain.hxx"
#include <api/Log.hpp>
#include <cstdint>
#include <span>
#include <string_view>
#include <vector>
#include <vulkan/vulkan.h>

namespace R3::vulkan {

PhysicalDevice::PhysicalDevice(const PhysicalDeviceSpecification& spec)
    : m_extensions(spec.extensions), // explicit copy
      m_sampleCount(undefined) {
    uint32 physicalDeviceCount;
    vkEnumeratePhysicalDevices(spec.instance, &physicalDeviceCount, nullptr);

    std::vector<VkPhysicalDevice> physicalDevices(physicalDeviceCount);
    vkEnumeratePhysicalDevices(spec.instance, &physicalDeviceCount, physicalDevices.data());

    VkPhysicalDevice physicalDevice;
    int32 bestScore = INT32_MIN;
    for (VkPhysicalDevice device : physicalDevices) {
        int32 score = evaluateDevice(device, spec.surface);

        if (score > bestScore) {
            physicalDevice = device;
            bestScore      = score;
        }
    }
    ENSURE(physicalDevice);

    VkPhysicalDeviceProperties physicalDeviceProperties;
    vkGetPhysicalDeviceProperties(physicalDevice, &physicalDeviceProperties);

    if (bestScore < 0) {
        LOG(Warning, "using suboptimal device {}", physicalDeviceProperties.deviceName);
    }

    // get samples as 8-bit flags (2^6 - 1)
    uint8 sampleFlags = static_cast<uint8>(physicalDeviceProperties.limits.framebufferColorSampleCounts &
                                           physicalDeviceProperties.limits.framebufferDepthSampleCounts);
    m_sampleCount     = 64;
    // shift sample count until we find biggest valid sampleCount
    while (!(m_sampleCount & sampleFlags) && m_sampleCount != 0) {
        m_sampleCount >>= 1;
    }

    ENSURE(m_sampleCount != 0);

    m_handle = physicalDevice;
}

uint32 PhysicalDevice::queryMemoryType(uint32 typeFilter, VkMemoryPropertyFlags propertyFlags) const {
    VkPhysicalDeviceMemoryProperties memoryProperties;
    vkGetPhysicalDeviceMemoryProperties(m_handle, &memoryProperties);

    for (uint32 i = 0; i < memoryProperties.memoryTypeCount; i++) {
        if ((typeFilter & (1 << i)) &&
            (memoryProperties.memoryTypes[i].propertyFlags & propertyFlags) == propertyFlags) {
            return i;
        }
    }

    ENSURE(false); /* unable to find suitable memory type */
}

int32 PhysicalDevice::evaluateDevice(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface) const {
    VkPhysicalDeviceProperties physicalDeviceProperties;
    vkGetPhysicalDeviceProperties(physicalDevice, &physicalDeviceProperties);

    VkPhysicalDeviceFeatures physicalDeviceFeatures;
    vkGetPhysicalDeviceFeatures(physicalDevice, &physicalDeviceFeatures);

    int32 deviceScore = 0;

    const auto deviceQueueIndices      = QueueFamilyIndices::query(physicalDevice, surface);
    const auto swapchainSupportDetails = SwapchainSupportDetails::query(physicalDevice, surface);

    deviceScore += deviceQueueIndices.isValid() ? 0 : -1000;
    deviceScore += swapchainSupportDetails.isValid() ? 0 : -1000;
    deviceScore += checkExtensionSupport(physicalDevice, m_extensions) ? 0 : -1000;
    deviceScore += physicalDeviceFeatures.samplerAnisotropy ? 0 : -1000;
    deviceScore += physicalDeviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU ? 0 : -100;

    return deviceScore;
}

bool PhysicalDevice::checkExtensionSupport(VkPhysicalDevice physicalDevice, std::span<const char* const> extensions) {
    uint32 extensionPropertiesCount;
    vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionPropertiesCount, nullptr);

    std::vector<VkExtensionProperties> extensionProperties(extensionPropertiesCount);
    vkEnumerateDeviceExtensionProperties(
        physicalDevice, nullptr, &extensionPropertiesCount, extensionProperties.data());

    for (const auto* requiredExtension : extensions) {
        auto it = std::ranges::find_if(extensionProperties, [=](const auto& extension) {
            return std::string_view(requiredExtension) == std::string_view(extension.extensionName);
        });

        if (it == extensionProperties.end()) {
            return false;
        }
    }

    return true;
}

} // namespace R3::vulkan
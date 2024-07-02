#include "vulkan-PhysicalDevice.hxx"

#include "vulkan-Instance.hxx"
#include "vulkan-Queue.hxx"
#include "vulkan-Surface.hxx"
#include "vulkan-Swapchain.hxx"
#include <api/Log.hpp>
#include <api/Result.hpp>
#include <api/Types.hpp>
#include <expected>
#include <span>
#include <stdlib.h>
#include <string_view>
#include <vector>
#include <vulkan/vulkan.h>

namespace R3::vulkan {

Result<PhysicalDevice> PhysicalDevice::query(const PhysicalDeviceSpecification& spec) {
    PhysicalDevice self;
    self.m_extensions = spec.extensions; // explicit copy

    uint32 physicalDeviceCount;
    (void)vkEnumeratePhysicalDevices(spec.instance.vk(), &physicalDeviceCount, nullptr);

    std::vector<VkPhysicalDevice> physicalDevices(physicalDeviceCount);
    (void)vkEnumeratePhysicalDevices(spec.instance.vk(), &physicalDeviceCount, physicalDevices.data());

    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    int32 bestScore                 = INT32_MIN;
    for (VkPhysicalDevice device : physicalDevices) {
        int32 score = self.evaluateDevice(device, spec.surface.vk());

        if (score > bestScore) {
            physicalDevice = device;
            bestScore      = score;
        }
    }

    if (physicalDevice == VK_NULL_HANDLE) {
        R3_LOG(Error, "no physical devices found");
        return std::unexpected(Error::InitializationFailure);
    }

    VkPhysicalDeviceProperties physicalDeviceProperties;
    vkGetPhysicalDeviceProperties(physicalDevice, &physicalDeviceProperties);

    if (bestScore < 0) {
        R3_LOG(Warning, "using suboptimal device {}", physicalDeviceProperties.deviceName);
    }

    // get samples as 8-bit flags (2^6 - 1)
    uint8 sampleFlags  = static_cast<uint8>(physicalDeviceProperties.limits.framebufferColorSampleCounts &
                                           physicalDeviceProperties.limits.framebufferDepthSampleCounts);
    self.m_sampleCount = 64;
    // shift sample count until we find biggest valid sampleCount
    while (!(self.m_sampleCount & sampleFlags) && self.m_sampleCount != 0) {
        self.m_sampleCount >>= 1;
    }

    if (self.m_sampleCount == 0) {
        R3_LOG(Error, "physical device sample count == 0");
        return std::unexpected(Error::InitializationFailure);
    }

    self.m_handle = physicalDevice;
    return self;
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

    R3_LOG(Error, "unable to find suitable memory type unrecoverable");
    std::exit(1);
}

int32 PhysicalDevice::evaluateDevice(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface) const {
    VkPhysicalDeviceProperties physicalDeviceProperties;
    vkGetPhysicalDeviceProperties(physicalDevice, &physicalDeviceProperties);

    VkPhysicalDeviceFeatures physicalDeviceFeatures;
    vkGetPhysicalDeviceFeatures(physicalDevice, &physicalDeviceFeatures);

    int32 deviceScore = 0;

    const auto deviceQueueIndices      = QueueFamilyIndices::query(physicalDevice, surface);
    const auto swapchainSupportDetails = SwapchainSupportDetails::query(physicalDevice, surface);

    if (!deviceQueueIndices) return INT32_MIN;
    if (!swapchainSupportDetails) return INT32_MIN;

    deviceScore += checkExtensionSupport(physicalDevice, m_extensions) ? 0 : -1000;
    deviceScore += physicalDeviceFeatures.samplerAnisotropy ? 0 : -1000;
    deviceScore += physicalDeviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU ? 0 : -100;

    return deviceScore;
}

bool PhysicalDevice::checkExtensionSupport(VkPhysicalDevice physicalDevice, std::span<const char* const> extensions) {
    uint32 extensionPropertiesCount;
    (void)vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionPropertiesCount, nullptr);

    std::vector<VkExtensionProperties> extensionProperties(extensionPropertiesCount);
    (void)vkEnumerateDeviceExtensionProperties(
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
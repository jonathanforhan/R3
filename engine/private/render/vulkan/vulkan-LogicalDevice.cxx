#if R3_VULKAN

#include "vulkan-LogicalDevice.hxx"
#include "vulkan-PhysicalDevice.hxx"
#include "vulkan-Queue.hxx"
#include <api/Log.hpp>
#include <api/Result.hpp>
#include <api/Types.hpp>
#include <expected>
#include <set>
#include <span>
#include <vector>
#include <vulkan/vulkan.h>

namespace R3::vulkan {

Result<LogicalDevice> LogicalDevice::create(const LogicalDeviceSpecification& spec) {
    LogicalDevice self;

    Result<QueueFamilyIndices> queueFamilyIndices = QueueFamilyIndices::query(spec.physicalDevice, spec.surface);
    R3_PROPAGATE(queueFamilyIndices);

    const std::set<uint32> uniqueQueueIndices = {
        queueFamilyIndices->graphics,
        queueFamilyIndices->presentation,
    };

    float queuePriority = 1.0f;

    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    for (uint32 index : uniqueQueueIndices) {
        queueCreateInfos.emplace_back(VkDeviceQueueCreateInfo{
            .sType            = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
            .pNext            = nullptr,
            .flags            = {},
            .queueFamilyIndex = index,
            .queueCount       = 1,
            .pQueuePriorities = &queuePriority,
        });
    }

    const VkPhysicalDeviceFeatures physicalDeviceFeatures = {
        .sampleRateShading = VK_TRUE,
        .samplerAnisotropy = VK_TRUE,
    };

    const std::span deviceExtensions = spec.physicalDevice.extensions();

    const VkDeviceCreateInfo logicalDeviceCreateInfo = {
        .sType                   = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .pNext                   = nullptr,
        .flags                   = {},
        .queueCreateInfoCount    = static_cast<uint32>(queueCreateInfos.size()),
        .pQueueCreateInfos       = queueCreateInfos.data(),
        .enabledLayerCount       = 0,
        .ppEnabledLayerNames     = nullptr,
        .enabledExtensionCount   = static_cast<uint32>(deviceExtensions.size()),
        .ppEnabledExtensionNames = deviceExtensions.empty() ? nullptr : deviceExtensions.data(),
        .pEnabledFeatures        = &physicalDeviceFeatures,
    };

    if (VkResult result = vkCreateDevice(spec.physicalDevice.vk(), &logicalDeviceCreateInfo, nullptr, &self.m_handle)) {
        R3_LOG(Error, "vkCreateDevice failure {}", (int)result);
        return std::unexpected(Error::InitializationFailure);
    }

    self.m_graphicsQueue.acquire({
        .device     = self,
        .queueType  = QueueType::Graphics,
        .queueIndex = static_cast<uint32>(queueFamilyIndices->graphics),
    });

    self.m_presentationQueue.acquire({
        .device     = self,
        .queueType  = QueueType::Presentation,
        .queueIndex = static_cast<uint32>(queueFamilyIndices->presentation),
    });

    return self;
}

LogicalDevice::~LogicalDevice() {
    vkDestroyDevice(m_handle, nullptr);
}

} // namespace R3::vulkan

#endif // R3_VULKAN

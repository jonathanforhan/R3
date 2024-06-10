#if R3_VULKAN

#include "api/Assert.hpp"
#include "vulkan-LogicalDevice.hxx"
#include "vulkan-PhysicalDevice.hxx"
#include "vulkan-Queue.hxx"
#include "vulkan-Surface.hxx"
#include <api/Types.hpp>
#include <set>
#include <span>
#include <vector>
#include <vulkan/vulkan.h>

namespace R3::vulkan {

LogicalDevice::LogicalDevice(const LogicalDeviceSpecification& spec) {
    const auto queueFamilyIndices = QueueFamilyIndices::query(spec.physicalDevice, spec.surface);
    ASSERT(queueFamilyIndices.isValid());

    const std::set<int32> uniqueQueueIndices = {
        queueFamilyIndices.graphics,
        queueFamilyIndices.presentation,
    };

    float queuePriority = 1.0f;

    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    for (int32 index : uniqueQueueIndices) {
        queueCreateInfos.emplace_back(VkDeviceQueueCreateInfo{
            .sType            = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
            .pNext            = nullptr,
            .flags            = {},
            .queueFamilyIndex = static_cast<uint32>(index),
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

    VkResult result = vkCreateDevice(spec.physicalDevice, &logicalDeviceCreateInfo, nullptr, &m_handle);
    ENSURE(result == VK_SUCCESS);

    m_graphicsQueue.acquire({
        .device     = *this,
        .queueType  = QueueType::Graphics,
        .queueIndex = static_cast<uint32>(queueFamilyIndices.graphics),
    });

    m_presentationQueue.acquire({
        .device     = *this,
        .queueType  = QueueType::Presentation,
        .queueIndex = static_cast<uint32>(queueFamilyIndices.presentation),
    });
}

LogicalDevice::~LogicalDevice() {
    vkDestroyDevice(m_handle, nullptr);
}

} // namespace R3::vulkan

#endif // R3_VULKAN

#if R3_VULKAN

#include "render/LogicalDevice.hpp"

#include <vulkan/vulkan.h>
#include <set>
#include <vector>
#include "api/Check.hpp"
#include "api/Ensure.hpp"
#include "api/Log.hpp"
#include "render/Instance.hpp"
#include "render/PhysicalDevice.hpp"
#include "render/Surface.hpp"

namespace R3 {

void LogicalDevice::create(const LogicalDeviceSpecification& spec) {
    CHECK(spec.instance != nullptr);
    CHECK(spec.surface != nullptr);
    CHECK(spec.physicalDevice != nullptr);
    m_spec = spec;

    auto queueFamilyIndices = QueueFamilyIndices::query(m_spec.physicalDevice->handle(), m_spec.surface->handle());
    CHECK(queueFamilyIndices.isValid());

    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    std::set<int32> uniqueQueueIndices = {
        queueFamilyIndices.graphics,
        queueFamilyIndices.presentation,
    };

    float queuePriority = 1.0f;

    for (uint32 index : uniqueQueueIndices) {
        queueCreateInfos.emplace_back(VkDeviceQueueCreateInfo{
            .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0U,
            .queueFamilyIndex = static_cast<uint32>(index),
            .queueCount = 1,
            .pQueuePriorities = &queuePriority,
        });
    }

    VkPhysicalDeviceFeatures physicalDeviceFeatures = {};

    const std::vector<const char*>& deviceExtensions = m_spec.physicalDevice->extensions();

    VkDeviceCreateInfo logicalDeviceCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0U,
        .queueCreateInfoCount = static_cast<uint32>(queueCreateInfos.size()),
        .pQueueCreateInfos = queueCreateInfos.data(),
        .enabledLayerCount = 0,
        .ppEnabledLayerNames = nullptr,
        .enabledExtensionCount = static_cast<uint32>(deviceExtensions.size()),
        .ppEnabledExtensionNames = deviceExtensions.empty() ? nullptr : deviceExtensions.data(),
        .pEnabledFeatures = &physicalDeviceFeatures};

    ENSURE(vkCreateDevice(m_spec.physicalDevice->handle<VkPhysicalDevice>(),
                          &logicalDeviceCreateInfo,
                          nullptr,
                          handlePtr<VkDevice*>()) == VK_SUCCESS);

    m_graphicsQueue.acquire({
        .logicalDevice = this,
        .queueType = QueueType::Graphics,
        .queueIndex = static_cast<uint32>(queueFamilyIndices.graphics),
    });

    m_presentationQueue.acquire({
        .logicalDevice = this,
        .queueType = QueueType::Presentation,
        .queueIndex = static_cast<uint32>(queueFamilyIndices.presentation),
    });
}

void LogicalDevice::destroy() {
    vkDestroyDevice(handle<VkDevice>(), nullptr);
}

} // namespace R3

#endif // R3_VULKAN

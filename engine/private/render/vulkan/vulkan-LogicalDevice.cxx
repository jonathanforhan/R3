#if R3_VULKAN

#include "render/LogicalDevice.hpp"

#include <vulkan/vulkan.h>
#include <set>
#include "api/Check.hpp"
#include "api/Ensure.hpp"
#include "render/Instance.hpp"
#include "render/PhysicalDevice.hpp"
#include "render/Surface.hpp"

namespace R3 {

LogicalDevice::LogicalDevice(const LogicalDeviceSpecification& spec)
    : m_spec(spec) {

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
            .flags = {},
            .queueFamilyIndex = static_cast<uint32>(index),
            .queueCount = 1,
            .pQueuePriorities = &queuePriority,
        });
    }

    VkPhysicalDeviceFeatures physicalDeviceFeatures = {
        .samplerAnisotropy = VK_TRUE,
    };

    std::span<const char* const> deviceExtensions = m_spec.physicalDevice->extensions();

    VkDeviceCreateInfo logicalDeviceCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .pNext = nullptr,
        .flags = {},
        .queueCreateInfoCount = static_cast<uint32>(queueCreateInfos.size()),
        .pQueueCreateInfos = queueCreateInfos.data(),
        .enabledLayerCount = 0,
        .ppEnabledLayerNames = nullptr,
        .enabledExtensionCount = static_cast<uint32>(deviceExtensions.size()),
        .ppEnabledExtensionNames = deviceExtensions.empty() ? nullptr : deviceExtensions.data(),
        .pEnabledFeatures = &physicalDeviceFeatures};

    VkDevice tmp;
    VkResult result = vkCreateDevice(m_spec.physicalDevice->as<VkPhysicalDevice>(), &logicalDeviceCreateInfo, nullptr, &tmp);
    ENSURE(result == VK_SUCCESS);
    setHandle(tmp);

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

LogicalDevice::~LogicalDevice() {
    if (validHandle()) {
        vkDestroyDevice(as<VkDevice>(), nullptr);
    }
}

} // namespace R3

#endif // R3_VULKAN

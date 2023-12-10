#if R3_VULKAN

#include "render/LogicalDevice.hpp"

#include <vulkan/vulkan.hpp>
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

    std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;
    std::set<int32> uniqueQueueIndices = {
        queueFamilyIndices.graphics,
        queueFamilyIndices.presentation,
    };

    float queuePriority = 1.0f;

    for (uint32 index : uniqueQueueIndices) {
        queueCreateInfos.emplace_back(vk::DeviceQueueCreateInfo{
            .sType = vk::StructureType::eDeviceQueueCreateInfo,
            .pNext = nullptr,
            .flags = {},
            .queueFamilyIndex = static_cast<uint32>(index),
            .queueCount = 1,
            .pQueuePriorities = &queuePriority,
        });
    }

    vk::PhysicalDeviceFeatures physicalDeviceFeatures = {};

    std::span<const char* const> deviceExtensions = m_spec.physicalDevice->extensions();

    vk::DeviceCreateInfo logicalDeviceCreateInfo = {
        .sType = vk::StructureType::eDeviceCreateInfo,
        .pNext = nullptr,
        .flags = {},
        .queueCreateInfoCount = static_cast<uint32>(queueCreateInfos.size()),
        .pQueueCreateInfos = queueCreateInfos.data(),
        .enabledLayerCount = 0,
        .ppEnabledLayerNames = nullptr,
        .enabledExtensionCount = static_cast<uint32>(deviceExtensions.size()),
        .ppEnabledExtensionNames = deviceExtensions.empty() ? nullptr : deviceExtensions.data(),
        .pEnabledFeatures = &physicalDeviceFeatures};

    setHandle(m_spec.physicalDevice->as<vk::PhysicalDevice>().createDevice(logicalDeviceCreateInfo));

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
        as<vk::Device>().destroy();
    }
}

} // namespace R3

#endif // R3_VULKAN

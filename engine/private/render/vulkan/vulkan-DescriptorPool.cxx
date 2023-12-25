#if R3_VULKAN

#include "render/DescriptorPool.hpp"

#include <vulkan/vulkan.hpp>
#include "render/LogicalDevice.hpp"

namespace R3 {

DescriptorPool::DescriptorPool(const DescriptorPoolSpecification& spec)
    : m_logicalDevice(&spec.logicalDevice) {
    m_layout = DescriptorSetLayout({*m_logicalDevice});

    const vk::DescriptorPoolSize uboDescriptorPoolSize = {
        .type = vk::DescriptorType::eUniformBuffer,
        .descriptorCount = spec.descriptorSetCount,
    };

    const vk::DescriptorPoolSize samplerDescriptorPoolSize = {
        .type = vk::DescriptorType::eCombinedImageSampler,
        .descriptorCount = spec.descriptorSetCount,
    };

    const vk::DescriptorPoolSize poolSizes[] = {
        uboDescriptorPoolSize,
        samplerDescriptorPoolSize,
    };

    const vk::DescriptorPoolCreateInfo descriptorPoolCreateInfo = {
        .sType = vk::StructureType::eDescriptorPoolCreateInfo,
        .pNext = nullptr,
        .flags = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet,
        .maxSets = spec.descriptorSetCount,
        .poolSizeCount = static_cast<uint32>(std::size(poolSizes)),
        .pPoolSizes = poolSizes,
    };

    setHandle(m_logicalDevice->as<vk::Device>().createDescriptorPool(descriptorPoolCreateInfo));

    m_descriptorSets = DescriptorSet::allocate({
        .logicalDevice = *m_logicalDevice,
        .descriptorPool = *this,
        .descriptorSetLayout = m_layout,
        .descriptorSetCount = spec.descriptorSetCount,
    });
}

DescriptorPool::~DescriptorPool() {
    if (validHandle()) {
        m_logicalDevice->as<vk::Device>().destroyDescriptorPool(as<vk::DescriptorPool>());
    }
}

} // namespace R3

#endif // R3_VULKAN
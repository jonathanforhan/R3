#if R3_VULKAN

#include <array>
#include <vulkan/vulkan.hpp>
#include "api/Check.hpp"
#include "render/DescriptorPool.hpp"
#include "render/LogicalDevice.hpp"
#include "render/RenderSpecification.hpp"

namespace R3 {

DescriptorPool::DescriptorPool(const DescriptorPoolSpecification& spec)
    : m_spec(spec) {
    m_layout = DescriptorSetLayout({m_spec.logicalDevice});

    const vk::DescriptorPoolSize uboDescriptorPoolSize = {
        .type = vk::DescriptorType::eUniformBuffer,
        .descriptorCount = m_spec.descriptorSetCount,
    };

    const vk::DescriptorPoolSize samplerDescriptorPoolSize = {
        .type = vk::DescriptorType::eCombinedImageSampler,
        .descriptorCount = m_spec.descriptorSetCount,
    };

    const std::array<vk::DescriptorPoolSize, 2> poolSizes = {
        uboDescriptorPoolSize,
        samplerDescriptorPoolSize,
    };

    const vk::DescriptorPoolCreateInfo descriptorPoolCreateInfo = {
        .sType = vk::StructureType::eDescriptorPoolCreateInfo,
        .pNext = nullptr,
        .flags = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet,
        .maxSets = m_spec.descriptorSetCount,
        .poolSizeCount = static_cast<uint32>(poolSizes.size()),
        .pPoolSizes = poolSizes.data(),
    };

    setHandle(m_spec.logicalDevice->as<vk::Device>().createDescriptorPool(descriptorPoolCreateInfo));

    m_descriptorSets = DescriptorSet::allocate({
        .logicalDevice = m_spec.logicalDevice,
        .descriptorPool = this,
        .descriptorSetLayout = &m_layout,
        .descriptorSetCount = m_spec.descriptorSetCount,
    });
}

DescriptorPool::~DescriptorPool() {
    if (validHandle()) {
        m_spec.logicalDevice->as<vk::Device>().destroyDescriptorPool(as<vk::DescriptorPool>());
    }
}

} // namespace R3

#endif // R3_VULKAN
#if R3_VULKAN

#include <vulkan/vulkan.hpp>
#include "api/Check.hpp"
#include "render/DescriptorPool.hpp"
#include "render/DescriptorSetLayout.hpp"
#include "render/LogicalDevice.hpp"
#include "render/RenderSpecification.hpp"

namespace R3 {

void DescriptorPool::create(const DescriptorPoolSpecification& spec) {
    CHECK(spec.logicalDevice != nullptr);
    CHECK(spec.descriptorSetLayout != nullptr);
    m_spec = spec;

    vk::DescriptorPoolSize descriptorPoolSize = {
        .type = vk::DescriptorType::eUniformBuffer,
        .descriptorCount = m_spec.descriptorSetCount,
    };
    
    vk::DescriptorPoolCreateInfo descriptorPoolCreateInfo = {
        .sType = vk::StructureType::eDescriptorPoolCreateInfo,
        .pNext = nullptr,
        .flags = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet,
        .maxSets = m_spec.descriptorSetCount,
        .poolSizeCount = 1,
        .pPoolSizes = &descriptorPoolSize,
    };

    setHandle(m_spec.logicalDevice->as<vk::Device>().createDescriptorPool(descriptorPoolCreateInfo));

    m_descriptorSets = DescriptorSet::allocate({
        .logicalDevice = m_spec.logicalDevice,
        .descriptorPool = this,
        .descriptorSetLayout = m_spec.descriptorSetLayout,
        .descriptorSetCount = m_spec.descriptorSetCount,
    });
}

void DescriptorPool::destroy() {
    DescriptorSet::free(m_descriptorSets);
    m_spec.logicalDevice->as<vk::Device>().destroyDescriptorPool(as<vk::DescriptorPool>());
}

} // namespace R3

#endif // R3_VULKAN
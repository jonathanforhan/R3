#if R3_VULKAN

#include <vulkan/vulkan.hpp>
#include "api/Check.hpp"
#include "render/DescriptorSetLayout.hpp"
#include "render/LogicalDevice.hpp"

namespace R3 {

DescriptorSetLayout::DescriptorSetLayout(const DescriptorSetLayoutSpecification& spec)
    : m_spec(spec) {

    vk::DescriptorSetLayoutBinding descriptorSetLayoutBinding = {
        .binding = 0,
        .descriptorType = vk::DescriptorType::eUniformBuffer,
        .descriptorCount = 1,
        .stageFlags = vk::ShaderStageFlagBits::eVertex,
        .pImmutableSamplers = nullptr,
    };

    vk::DescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = {
        .sType = vk::StructureType::eDescriptorSetLayoutCreateInfo,
        .pNext = nullptr,
        .flags = {},
        .bindingCount = 1,
        .pBindings = &descriptorSetLayoutBinding,
    };

    setHandle(m_spec.logicalDevice->as<vk::Device>().createDescriptorSetLayout(descriptorSetLayoutCreateInfo));
}

DescriptorSetLayout::~DescriptorSetLayout() {
    if (validHandle()) {
        m_spec.logicalDevice->as<vk::Device>().destroyDescriptorSetLayout(as<vk::DescriptorSetLayout>());
    }
}

} // namespace R3

#endif // R3_VULKAN

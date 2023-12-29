#if R3_VULKAN

#include <vulkan/vulkan.hpp>
#include "render/DescriptorSetLayout.hxx"
#include "render/LogicalDevice.hxx"

namespace R3 {

DescriptorSetLayout::DescriptorSetLayout(const DescriptorSetLayoutSpecification& spec)
    : m_logicalDevice(&spec.logicalDevice) {
    std::vector<vk::DescriptorSetLayoutBinding> descriptorSetLayoutBindings;

    for (const auto& binding : spec.layoutBindings) {
        auto& vkBinding = descriptorSetLayoutBindings.emplace_back();
        vkBinding.binding = binding.binding;
        vkBinding.descriptorType = vk::DescriptorType(binding.type);
        vkBinding.descriptorCount = binding.count;
        vkBinding.stageFlags = vk::ShaderStageFlagBits(binding.stage);
    };

    const vk::DescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = {
        .sType = vk::StructureType::eDescriptorSetLayoutCreateInfo,
        .pNext = nullptr,
        .flags = {},
        .bindingCount = static_cast<uint32>(descriptorSetLayoutBindings.size()),
        .pBindings = descriptorSetLayoutBindings.data(),
    };

    setHandle(m_logicalDevice->as<vk::Device>().createDescriptorSetLayout(descriptorSetLayoutCreateInfo));
}

DescriptorSetLayout::~DescriptorSetLayout() {
    if (validHandle()) {
        m_logicalDevice->as<vk::Device>().destroyDescriptorSetLayout(as<vk::DescriptorSetLayout>());
    }
}

} // namespace R3

#endif // R3_VULKAN

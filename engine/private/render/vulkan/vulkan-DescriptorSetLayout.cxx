#if R3_VULKAN

#include <vulkan/vulkan.hpp>
#include "render/DescriptorSetLayout.hpp"
#include "render/LogicalDevice.hpp"

namespace R3 {

DescriptorSetLayout::DescriptorSetLayout(const DescriptorSetLayoutSpecification& spec)
    : m_logicalDevice(&spec.logicalDevice) {
    std::vector<vk::DescriptorSetLayoutBinding> descriptorSetLayoutBindings(spec.layoutBindings.size());
    std::ranges::transform(spec.layoutBindings, descriptorSetLayoutBindings.begin(), [](const auto& binding) {
        return vk::DescriptorSetLayoutBinding{
            .binding = binding.binding,
            .descriptorType = vk::DescriptorType(binding.type),
            .descriptorCount = binding.count,
            .stageFlags = vk::ShaderStageFlagBits(binding.stage),
        };
    });

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

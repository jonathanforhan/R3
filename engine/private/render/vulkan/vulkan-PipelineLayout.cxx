#if R3_VULKAN

#include "render/PipelineLayout.hxx"

#include <vulkan/vulkan.hpp>
#include "render/DescriptorSetLayout.hxx"
#include "render/LogicalDevice.hxx"
#include "vulkan-PushConstant.hxx"

namespace R3 {

PipelineLayout::PipelineLayout(const PipelineLayoutSpecification& spec)
    : m_logicalDevice(&spec.logicalDevice) {
    vk::DescriptorSetLayout descriptorSetLayouts[] = {
        spec.descriptorSetLayout.as<vk::DescriptorSetLayout>(),
    };

    vk::PushConstantRange pushConstant = {
        .stageFlags = vk::ShaderStageFlagBits::eVertex,
        .offset = 0,
        .size = sizeof(vulkan::PushConstant),
    };

    vk::PipelineLayoutCreateInfo pipelineLayoutCreateInfo = {
        .sType = vk::StructureType::ePipelineLayoutCreateInfo,
        .pNext = nullptr,
        .flags = {},
        .setLayoutCount = 1,
        .pSetLayouts = descriptorSetLayouts,
        .pushConstantRangeCount = 1,
        .pPushConstantRanges = &pushConstant,
    };

    setHandle(m_logicalDevice->as<vk::Device>().createPipelineLayout(pipelineLayoutCreateInfo));
}

PipelineLayout::~PipelineLayout() {
    if (validHandle()) {
        m_logicalDevice->as<vk::Device>().destroyPipelineLayout(as<vk::PipelineLayout>());
    }
}

} // namespace R3

#endif // R3_VULKAN
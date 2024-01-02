#if R3_VULKAN

#include "render/PipelineLayout.hxx"

#include <vulkan/vulkan.hpp>
#include "render/DescriptorSetLayout.hxx"
#include "render/LogicalDevice.hxx"
#include "render/ShaderObjects.hxx"

namespace R3 {

PipelineLayout::PipelineLayout(const PipelineLayoutSpecification& spec)
    : m_logicalDevice(&spec.logicalDevice) {
    vk::DescriptorSetLayout descriptorSetLayouts[] = {
        spec.descriptorSetLayout.as<vk::DescriptorSetLayout>(),
    };

    vk::PushConstantRange pushConstants[] = {
        {
            .stageFlags = vk::ShaderStageFlagBits::eFragment,
            .offset = 0,
            .size = sizeof(FragmentPushConstant),
        },
    };

    vk::PipelineLayoutCreateInfo pipelineLayoutCreateInfo = {
        .sType = vk::StructureType::ePipelineLayoutCreateInfo,
        .pNext = nullptr,
        .flags = {},
        .setLayoutCount = 1,
        .pSetLayouts = descriptorSetLayouts,
        .pushConstantRangeCount = uint32(std::size(pushConstants)),
        .pPushConstantRanges = pushConstants,
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
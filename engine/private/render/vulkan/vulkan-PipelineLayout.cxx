#if R3_VULKAN

#include "render/PipelineLayout.hpp"

#include <vulkan/vulkan.hpp>
#include "api/Check.hpp"
#include "render/DescriptorSetLayout.hpp"
#include "render/LogicalDevice.hpp"

namespace R3 {

void PipelineLayout::create(const PipelineLayoutSpecification& spec) {
    CHECK(spec.logicalDevice != nullptr);
    CHECK(spec.descriptorSetLayout != nullptr);
    m_spec = spec;

    vk::DescriptorSetLayout descriptorSetLayouts[] = {
        m_spec.descriptorSetLayout->as<vk::DescriptorSetLayout>(),
    };

    vk::PipelineLayoutCreateInfo pipelineLayoutCreateInfo = {
        .sType = vk::StructureType::ePipelineLayoutCreateInfo,
        .pNext = nullptr,
        .flags = {},
        .setLayoutCount = 1,
        .pSetLayouts = descriptorSetLayouts,
        .pushConstantRangeCount = 0,
        .pPushConstantRanges = nullptr,
    };

    setHandle(m_spec.logicalDevice->as<vk::Device>().createPipelineLayout(pipelineLayoutCreateInfo));
}

void PipelineLayout::destroy() {
    m_spec.logicalDevice->as<vk::Device>().destroyPipelineLayout(as<vk::PipelineLayout>());
}

} // namespace R3

#endif // R3_VULKAN
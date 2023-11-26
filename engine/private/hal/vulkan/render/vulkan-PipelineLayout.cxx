#if R3_VULKAN

#include "render/PipelineLayout.hpp"

#include <vulkan/vulkan.h>
#include "api/Check.hpp"
#include "api/Ensure.hpp"
#include "render/LogicalDevice.hpp"

namespace R3 {

void PipelineLayout::create(const PipelineLayoutSpecification& spec) {
    CHECK(spec.logicalDevice != nullptr);
    m_spec = spec;

    VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0U,
        .setLayoutCount = 0,
        .pSetLayouts = nullptr,
        .pushConstantRangeCount = 0,
        .pPushConstantRanges = nullptr,
    };

    ENSURE(vkCreatePipelineLayout(m_spec.logicalDevice->handle<VkDevice>(),
                                  &pipelineLayoutCreateInfo,
                                  nullptr,
                                  handlePtr<VkPipelineLayout*>()) == VK_SUCCESS);
}

void PipelineLayout::destroy() {
    vkDestroyPipelineLayout(m_spec.logicalDevice->handle<VkDevice>(), handle<VkPipelineLayout>(), nullptr);
}

} // namespace R3

#endif // R3_VULKAN
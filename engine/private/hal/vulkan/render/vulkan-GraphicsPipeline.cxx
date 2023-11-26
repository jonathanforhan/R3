#if R3_VULKAN

#include "render/GraphicsPipeline.hpp"

#include <vulkan/vulkan.h>
#include <vector>
#include "api/Check.hpp"
#include "render/LogicalDevice.hpp"
#include "render/Swapchain.hpp"

namespace R3 {

void GraphicsPipeline::create(const GraphicsPipelineSpecification& spec) {
    CHECK(spec.logicalDevice != nullptr);
    CHECK(spec.swapchain != nullptr);
    m_spec = spec;

    m_vertexShader.create({
        .logicalDevice = m_spec.logicalDevice,
        .path = m_spec.vertexShaderPath
    });

    m_fragmentShader.create({
        .logicalDevice = m_spec.logicalDevice,
        .path = m_spec.fragmentShaderPath
    });

    VkPipelineShaderStageCreateInfo vertexShaderStageCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0U,
        .stage = VK_SHADER_STAGE_VERTEX_BIT,
        .module = m_vertexShader.handle<VkShaderModule>(),
        .pName = "main",
        .pSpecializationInfo = nullptr,
    };

    VkPipelineShaderStageCreateInfo fragmentShaderStageCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0U,
        .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
        .module = m_fragmentShader.handle<VkShaderModule>(),
        .pName = "main",
        .pSpecializationInfo = nullptr,
    };

    VkPipelineShaderStageCreateInfo shaderStageCreateInfos[] = {
        vertexShaderStageCreateInfo,
        fragmentShaderStageCreateInfo,
    };

    std::vector<VkDynamicState> dynamicStates = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR,
    };

    VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0U,
        .dynamicStateCount = static_cast<uint32>(dynamicStates.size()),
        .pDynamicStates = dynamicStates.data(),
    };

    VkPipelineVertexInputStateCreateInfo vertexInputStateCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0U,
        .vertexBindingDescriptionCount = 0,
        .pVertexBindingDescriptions = nullptr,
        .vertexAttributeDescriptionCount = 0,
        .pVertexAttributeDescriptions = nullptr,
    };

    VkPipelineInputAssemblyStateCreateInfo inputAssemblyStateCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0U,
        .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
        .primitiveRestartEnable = VK_FALSE,
    };

    VkViewport viewport = {
        .x = 0.0f,
        .y = 0.0f,
        .width = m_spec.swapchain->extent().x,
        .height = m_spec.swapchain->extent().y,
        .minDepth = 0.0f,
        .maxDepth = 1.0f,
    };
}

void GraphicsPipeline::destroy() {
    m_vertexShader.destroy();
    m_fragmentShader.destroy();
}

} // namespace R3

#endif // R3_VULKAN

#if R3_VULKAN

#include "vulkan-GraphicsPipeline.hxx"

#include "vulkan-LogicalDevice.hxx"
#include "vulkan-PhysicalDevice.hxx"
#include "vulkan-RenderPass.hxx"
#include "vulkan-ShaderModule.hxx"
#include "vulkan-VulkanObject.hxx"
#include <api/Assert.hpp>
#include <api/Types.hpp>
#include <cstdint>
#include <vector>
#include <vulkan/vulkan.h>

namespace R3::vulkan {

GraphicsPipeline::GraphicsPipeline(const GraphicsPipelineSpecification& spec)
    : m_device(spec.device.vk()),
      m_vertexShader(std::move(spec.vertexShader)),
      m_fragmentShader(std::move(spec.fragmentShader)) {
    // Pipeline Layout
    const VkDescriptorSetLayout descriptorSetLayouts[] = {
        spec.descriptorSetLayout,
    };

    // VkPushConstantRange pushConstants[] = { }; TODO?

    const VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {
        .sType                  = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .pNext                  = nullptr,
        .flags                  = {},
        .setLayoutCount         = static_cast<uint32>(std::size(descriptorSetLayouts)),
        .pSetLayouts            = descriptorSetLayouts,
        .pushConstantRangeCount = 0,
        .pPushConstantRanges    = nullptr, // TODO?
    };

    VkPipelineLayout layout;
    VkResult result = vkCreatePipelineLayout(m_device, &pipelineLayoutCreateInfo, nullptr, &layout);
    ENSURE(result == VK_SUCCESS);
    VulkanObject<VkPipelineLayout>::m_handle = layout;

    // Shaders
    auto pipelineShaderStageCreateInfoGenerator = [](const ShaderModule& shader) {
        return VkPipelineShaderStageCreateInfo{
            .sType               = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            .pNext               = nullptr,
            .flags               = {},
            .stage               = shader.stage(),
            .module              = shader.vk(),
            .pName               = "main",
            .pSpecializationInfo = nullptr,
        };
    };

    std::vector<VkPipelineShaderStageCreateInfo> shaderStageCreateInfos = {
        pipelineShaderStageCreateInfoGenerator(m_vertexShader),
        pipelineShaderStageCreateInfoGenerator(m_fragmentShader),
    };

    // Pipeline
    const VkPipelineVertexInputStateCreateInfo vertexInputStateCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
        .pNext = nullptr,
        .flags = {},
#if 1 // TODO FIXME
        .vertexBindingDescriptionCount = 0,
#else
        .vertexBindingDescriptionCount = 1,
        .pVertexBindingDescriptions    = &spec.bindingDescription,
#endif
        .vertexAttributeDescriptionCount = static_cast<uint32_t>(spec.attributeDescriptions.size()),
        .pVertexAttributeDescriptions    = spec.attributeDescriptions.data(),
    };

    constexpr VkPipelineInputAssemblyStateCreateInfo inputAssemblyStateCreateInfo = {
        .sType                  = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
        .pNext                  = nullptr,
        .flags                  = {},
        .topology               = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
        .primitiveRestartEnable = VK_FALSE,
    };

    constexpr VkDynamicState dynamicStates[] = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR,
    };

    const VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo = {
        .sType             = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
        .pNext             = nullptr,
        .flags             = {},
        .dynamicStateCount = static_cast<uint32_t>(std::size(dynamicStates)),
        .pDynamicStates    = dynamicStates,
    };

    constexpr VkPipelineViewportStateCreateInfo viewportStateCreateInfo = {
        .sType         = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
        .pNext         = nullptr,
        .flags         = {},
        .viewportCount = 1,
        .pViewports    = nullptr,
        .scissorCount  = 1,
        .pScissors     = nullptr,
    };

    constexpr VkPipelineRasterizationStateCreateInfo rasterizationStateCreateInfo = {
        .sType                   = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
        .pNext                   = nullptr,
        .flags                   = {},
        .depthClampEnable        = VK_FALSE,
        .rasterizerDiscardEnable = VK_FALSE,
        .polygonMode             = VK_POLYGON_MODE_FILL,
        .cullMode                = VK_CULL_MODE_BACK_BIT,
        .frontFace               = VK_FRONT_FACE_COUNTER_CLOCKWISE,
        .depthBiasEnable         = VK_FALSE,
        .depthBiasConstantFactor = 0.0f,
        .depthBiasClamp          = 0.0f,
        .depthBiasSlopeFactor    = 0.0f,
        .lineWidth               = 1.0f,
    };

    const VkPipelineMultisampleStateCreateInfo multisampleStateCreateInfo = {
        .sType                 = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
        .pNext                 = nullptr,
        .flags                 = {},
        .rasterizationSamples  = spec.msaa ? spec.physicalDevice.sampleCount() : VK_SAMPLE_COUNT_1_BIT,
        .sampleShadingEnable   = spec.msaa ? VK_TRUE : VK_FALSE,
        .minSampleShading      = 1.0f,
        .pSampleMask           = nullptr,
        .alphaToCoverageEnable = VK_FALSE,
        .alphaToOneEnable      = VK_FALSE,
    };

    constexpr VkPipelineDepthStencilStateCreateInfo depthStencilStateCreateInfo = {
        .sType                 = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
        .pNext                 = nullptr,
        .flags                 = {},
        .depthTestEnable       = VK_TRUE,
        .depthWriteEnable      = VK_TRUE,
        .depthCompareOp        = VK_COMPARE_OP_LESS,
        .depthBoundsTestEnable = VK_FALSE,
        .stencilTestEnable     = VK_FALSE,
        .front                 = {},
        .back                  = {},
        .minDepthBounds        = 0.0f,
        .maxDepthBounds        = 0.0f,
    };

    constexpr VkPipelineColorBlendAttachmentState colorBlendAttachmentState = {
        .blendEnable         = VK_FALSE,
        .srcColorBlendFactor = VK_BLEND_FACTOR_ONE,
        .dstColorBlendFactor = VK_BLEND_FACTOR_ZERO,
        .colorBlendOp        = VK_BLEND_OP_ADD,
        .srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
        .dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
        .alphaBlendOp        = VK_BLEND_OP_ADD,
        .colorWriteMask =
            VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
    };

    const VkPipelineColorBlendStateCreateInfo colorBlendStateCreateInfo = {
        .sType           = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
        .pNext           = nullptr,
        .flags           = {},
        .logicOpEnable   = VK_FALSE,
        .logicOp         = VK_LOGIC_OP_COPY,
        .attachmentCount = 1,
        .pAttachments    = &colorBlendAttachmentState,
        .blendConstants =
            {
                0.0f,
                0.0f,
                0.0f,
                0.0f,
            },
    };

    const VkGraphicsPipelineCreateInfo graphicsPipelineCreateInfo = {
        .sType               = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
        .pNext               = nullptr,
        .flags               = {},
        .stageCount          = static_cast<uint32>(shaderStageCreateInfos.size()),
        .pStages             = shaderStageCreateInfos.data(),
        .pVertexInputState   = &vertexInputStateCreateInfo,
        .pInputAssemblyState = &inputAssemblyStateCreateInfo,
        .pTessellationState  = nullptr, // TODO ??
        .pViewportState      = &viewportStateCreateInfo,
        .pRasterizationState = &rasterizationStateCreateInfo,
        .pMultisampleState   = &multisampleStateCreateInfo,
        .pDepthStencilState  = &depthStencilStateCreateInfo,
        .pColorBlendState    = &colorBlendStateCreateInfo,
        .pDynamicState       = &dynamicStateCreateInfo,
        .layout              = VulkanObject<VkPipelineLayout>::m_handle,
        .renderPass          = spec.renderPass.vk(),
        .subpass             = 0,
        .basePipelineHandle  = VK_NULL_HANDLE,
        .basePipelineIndex   = -1,
    };

    VkPipeline pipeline;
    result = vkCreateGraphicsPipelines(m_device, nullptr, 1, &graphicsPipelineCreateInfo, nullptr, &pipeline);
    ENSURE(result == VK_SUCCESS);
    VulkanObject<VkPipeline>::m_handle = pipeline;
}

GraphicsPipeline::~GraphicsPipeline() {
    vkDestroyPipelineLayout(m_device, VulkanObject<VkPipelineLayout>::m_handle, nullptr);
    vkDestroyPipeline(m_device, VulkanObject<VkPipeline>::m_handle, nullptr);
}

} // namespace R3::vulkan

#endif // R3_VULKAN

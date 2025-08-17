#if R3_VULKAN

#include "render/GraphicsPipeline.hpp"

#include <cstddef>
#include <cstdint>
#include <iterator>
#include <span>
#include <vector>
#include <vulkan/vulkan_core.h>
#include "Exception.hpp"
#include "Types.hpp"
#include "render/RenderContext.hpp"
#include "render/RenderPass.hpp"
#include "render/Shader.hpp"
#include "vulkan-Check.hpp"

namespace R3 {

VkVertexInputBindingDescription Vertex::getBindingDescription() noexcept {
    const VkVertexInputBindingDescription vertexInputBindingDescription = {
        .binding   = 0,
        .stride    = sizeof(Vertex),
        .inputRate = VK_VERTEX_INPUT_RATE_VERTEX,
    };
    return vertexInputBindingDescription;
}

std::vector<VkVertexInputAttributeDescription> Vertex::getAttributeDescriptions() noexcept {
    const std::vector<VkVertexInputAttributeDescription> vertexInputAttributeDescription = {
        // Position attribute
        {
            .location = 0,
            .binding  = 0,
            .format   = VK_FORMAT_R32G32B32_SFLOAT,
            .offset   = offsetof(Vertex, position),
        },
        // Color attribute
        {
            .location = 1,
            .binding  = 0,
            .format   = VK_FORMAT_R32G32B32_SFLOAT,
            .offset   = offsetof(Vertex, color),
        },
    };
    return vertexInputAttributeDescription;
}

void GraphicsPipeline::create(RenderContext& ctx,
                              RenderPass& renderPass,
                              Shader& vertexShader,
                              Shader& fragmentShader,
                              std::span<const VkDescriptorSetLayout> layouts) {
    m_device = ctx.device();

    const VkPipelineShaderStageCreateInfo vertShaderStageInfo = {
        .sType               = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
        .pNext               = nullptr,
        .flags               = 0,
        .stage               = (VkShaderStageFlagBits)vertexShader.type(),
        .module              = vertexShader.handle(),
        .pName               = "main",
        .pSpecializationInfo = nullptr,
    };

    const VkPipelineShaderStageCreateInfo fragShaderStageInfo = {
        .sType               = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
        .pNext               = nullptr,
        .flags               = 0,
        .stage               = (VkShaderStageFlagBits)fragmentShader.type(),
        .module              = fragmentShader.handle(),
        .pName               = "main",
        .pSpecializationInfo = nullptr,
    };

    const VkPipelineShaderStageCreateInfo shaderStagesInfo[] = {vertShaderStageInfo, fragShaderStageInfo};

    const auto bindingDescription    = Vertex::getBindingDescription();
    const auto attributeDescriptions = Vertex::getAttributeDescriptions();

    const VkPipelineVertexInputStateCreateInfo vertexInputStateInfo = {
        .sType                           = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
        .pNext                           = nullptr,
        .flags                           = 0,
        .vertexBindingDescriptionCount   = 1,
        .pVertexBindingDescriptions      = &bindingDescription,
        .vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size()),
        .pVertexAttributeDescriptions    = attributeDescriptions.data(),
    };

    const VkPipelineInputAssemblyStateCreateInfo inputAssemblyStateInfo = {
        .sType                  = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
        .pNext                  = nullptr,
        .flags                  = 0,
        .topology               = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
        .primitiveRestartEnable = VK_FALSE,
    };

    const VkDynamicState dynamicStates[] = {
        VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR, VK_DYNAMIC_STATE_CULL_MODE, VK_DYNAMIC_STATE_FRONT_FACE, VK_DYNAMIC_STATE_LINE_WIDTH,
        // VK_DYNAMIC_STATE_DEPTH_BIAS,
        // VK_DYNAMIC_STATE_BLEND_CONSTANTS,
        // VK_DYNAMIC_STATE_STENCIL_COMPARE_MASK,
        // VK_DYNAMIC_STATE_STENCIL_WRITE_MASK,
        // VK_DYNAMIC_STATE_STENCIL_REFERENCE,
    };

    const VkPipelineDynamicStateCreateInfo dynamicStateInfo = {
        .sType             = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
        .pNext             = nullptr,
        .flags             = 0,
        .dynamicStateCount = static_cast<uint32_t>(std::size(dynamicStates)),
        .pDynamicStates    = dynamicStates,
    };

    const VkPipelineViewportStateCreateInfo viewportStateInfo = {
        .sType         = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
        .pNext         = nullptr,
        .flags         = 0,
        .viewportCount = 1,
        .pViewports    = nullptr, // dynamic
        .scissorCount  = 1,
        .pScissors     = nullptr, // dynamic
    };

    const VkPipelineRasterizationStateCreateInfo rasterizationStateInfo = {
        .sType                   = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
        .pNext                   = nullptr,
        .flags                   = 0,
        .depthClampEnable        = VK_FALSE,
        .rasterizerDiscardEnable = VK_FALSE,
        .polygonMode             = VK_POLYGON_MODE_FILL,
        .cullMode                = VK_CULL_MODE_BACK_BIT,           // dynamic
        .frontFace               = VK_FRONT_FACE_COUNTER_CLOCKWISE, // dynamic
        .depthBiasEnable         = VK_FALSE,
        .depthBiasConstantFactor = 0.0f,
        .depthBiasClamp          = 0.0f,
        .depthBiasSlopeFactor    = 0.0f,
        .lineWidth               = 1.0f,
    };

    const VkPipelineMultisampleStateCreateInfo multisampeStateInfo = {
        .sType                 = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
        .pNext                 = nullptr,
        .flags                 = 0,
        .rasterizationSamples  = VK_SAMPLE_COUNT_1_BIT,
        .sampleShadingEnable   = VK_FALSE,
        .minSampleShading      = 0.0f,
        .pSampleMask           = nullptr,
        .alphaToCoverageEnable = VK_FALSE,
        .alphaToOneEnable      = VK_FALSE,
    };

    const VkPipelineColorBlendAttachmentState colorBlendAttachmentState = {
        .blendEnable         = VK_FALSE,
        .srcColorBlendFactor = VK_BLEND_FACTOR_ZERO,
        .dstColorBlendFactor = VK_BLEND_FACTOR_ZERO,
        .colorBlendOp        = VK_BLEND_OP_ADD,
        .srcAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
        .dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
        .alphaBlendOp        = VK_BLEND_OP_ADD,
        .colorWriteMask      = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
    };

    const VkPipelineColorBlendStateCreateInfo colorBlendStateInfo = {
        .sType           = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
        .pNext           = nullptr,
        .flags           = 0,
        .logicOpEnable   = VK_FALSE,
        .logicOp         = VK_LOGIC_OP_COPY,
        .attachmentCount = 1,
        .pAttachments    = &colorBlendAttachmentState,
        .blendConstants  = {0.0f, 0.0f, 0.0f, 0.0f},
    };

    const VkPipelineLayoutCreateInfo pipelineLayoutInfo = {
        .sType                  = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .pNext                  = nullptr,
        .flags                  = 0,
        .setLayoutCount         = static_cast<uint32>(layouts.size()),
        .pSetLayouts            = layouts.data(),
        .pushConstantRangeCount = 0,
        .pPushConstantRanges    = nullptr,
    };
    VK_CHECK(vkCreatePipelineLayout(m_device, &pipelineLayoutInfo, nullptr, &m_pipelineLayout));

    const VkGraphicsPipelineCreateInfo graphicsPipelineInfo = {
        .sType               = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
        .pNext               = nullptr,
        .flags               = 0,
        .stageCount          = 2,
        .pStages             = shaderStagesInfo,
        .pVertexInputState   = &vertexInputStateInfo,
        .pInputAssemblyState = &inputAssemblyStateInfo,
        .pTessellationState  = nullptr,
        .pViewportState      = &viewportStateInfo,
        .pRasterizationState = &rasterizationStateInfo,
        .pMultisampleState   = &multisampeStateInfo,
        .pDepthStencilState  = nullptr,
        .pColorBlendState    = &colorBlendStateInfo,
        .pDynamicState       = &dynamicStateInfo,
        .layout              = m_pipelineLayout,
        .renderPass          = renderPass.handle(),
        .subpass             = 0,
        .basePipelineHandle  = VK_NULL_HANDLE,
        .basePipelineIndex   = -1,
    };

    try {
        VK_CHECK(vkCreateGraphicsPipelines(m_device, VK_NULL_HANDLE, 1, &graphicsPipelineInfo, nullptr, &m_pipeline));
    } catch (const Exception& ex) {
        vkDestroyPipelineLayout(m_device, m_pipelineLayout, nullptr);
        throw ex;
    }
}

void GraphicsPipeline::destroy() noexcept {
    if (m_pipeline != VK_NULL_HANDLE && m_device != VK_NULL_HANDLE) {
        vkDestroyPipeline(m_device, m_pipeline, nullptr);
        m_pipeline = VK_NULL_HANDLE;
    }

    if (m_pipelineLayout != VK_NULL_HANDLE && m_device != VK_NULL_HANDLE) {
        vkDestroyPipelineLayout(m_device, m_pipelineLayout, nullptr);
        m_pipelineLayout = VK_NULL_HANDLE;
    }

    m_device = VK_NULL_HANDLE;
}

void GraphicsPipeline::bind(VkCommandBuffer commandBuffer) const {
    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline);
}

void GraphicsPipeline::setViewport(VkCommandBuffer cmd, const VkViewport& viewport) {
    vkCmdSetViewport(cmd, 0, 1, &viewport);
}

void GraphicsPipeline::setScissor(VkCommandBuffer cmd, const VkRect2D& scissor) {
    vkCmdSetScissor(cmd, 0, 1, &scissor);
}

void GraphicsPipeline::setCullMode(VkCommandBuffer cmd, VkCullModeFlags cullMode) {
    vkCmdSetCullMode(cmd, cullMode);
}

void GraphicsPipeline::setFrontFace(VkCommandBuffer cmd, VkFrontFace frontFace) {
    vkCmdSetFrontFace(cmd, frontFace);
}

void GraphicsPipeline::setLineWidth(VkCommandBuffer cmd, float lineWidth) {
    vkCmdSetLineWidth(cmd, lineWidth);
}

} // namespace R3

#endif // R3_VULKAN
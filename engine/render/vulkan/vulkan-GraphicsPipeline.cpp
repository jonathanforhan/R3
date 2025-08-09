#if R3_VULKAN

#include "render/GraphicsPipeline.hpp"

#include <cstddef>
#include <cstdint>
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
                              uvec2 extent) {
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

    const VkPipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo, fragShaderStageInfo};

    const auto bindingDescription    = Vertex::getBindingDescription();
    const auto attributeDescriptions = Vertex::getAttributeDescriptions();

    const VkPipelineVertexInputStateCreateInfo vertexInputInfo = {
        .sType                           = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
        .pNext                           = nullptr,
        .flags                           = 0,
        .vertexBindingDescriptionCount   = 1,
        .pVertexBindingDescriptions      = &bindingDescription,
        .vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size()),
        .pVertexAttributeDescriptions    = attributeDescriptions.data(),
    };

    const VkPipelineInputAssemblyStateCreateInfo inputAssembly = {
        .sType                  = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
        .pNext                  = nullptr,
        .flags                  = 0,
        .topology               = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
        .primitiveRestartEnable = VK_FALSE,
    };

    VkExtent2D extent2D{extent.x, extent.y};

    const VkViewport viewport = {
        .x        = 0.0f,
        .y        = 0.0f,
        .width    = static_cast<float>(extent2D.width),
        .height   = static_cast<float>(extent2D.height),
        .minDepth = 0.0f,
        .maxDepth = 1.0f,
    };

    const VkRect2D scissor = {
        .offset = {0, 0},
        .extent = extent2D,
    };

    const VkPipelineViewportStateCreateInfo viewportState = {
        .sType         = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
        .pNext         = nullptr,
        .flags         = 0,
        .viewportCount = 1,
        .pViewports    = &viewport,
        .scissorCount  = 1,
        .pScissors     = &scissor,
    };

    const VkPipelineRasterizationStateCreateInfo rasterizer = {
        .sType                   = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
        .pNext                   = nullptr,
        .flags                   = 0,
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

    const VkPipelineMultisampleStateCreateInfo multisampling = {
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

    const VkPipelineColorBlendAttachmentState colorBlendAttachment = {
        .blendEnable         = VK_FALSE,
        .srcColorBlendFactor = VK_BLEND_FACTOR_ZERO,
        .dstColorBlendFactor = VK_BLEND_FACTOR_ZERO,
        .colorBlendOp        = VK_BLEND_OP_ADD,
        .srcAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
        .dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
        .alphaBlendOp        = VK_BLEND_OP_ADD,
        .colorWriteMask =
            VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
    };

    const VkPipelineColorBlendStateCreateInfo colorBlending = {
        .sType           = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
        .pNext           = nullptr,
        .flags           = 0,
        .logicOpEnable   = VK_FALSE,
        .logicOp         = VK_LOGIC_OP_COPY,
        .attachmentCount = 1,
        .pAttachments    = &colorBlendAttachment,
        .blendConstants =
            {
                0.0f,
                0.0f,
                0.0f,
                0.0f,
            },
    };

    const VkPipelineLayoutCreateInfo pipelineLayoutInfo = {
        .sType                  = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .pNext                  = nullptr,
        .flags                  = 0,
        .setLayoutCount         = 0,
        .pSetLayouts            = nullptr,
        .pushConstantRangeCount = 0,
        .pPushConstantRanges    = nullptr,
    };
    VK_CHECK(vkCreatePipelineLayout(m_device, &pipelineLayoutInfo, nullptr, &m_pipelineLayout));

    const VkGraphicsPipelineCreateInfo graphicsPipelineInfo = {
        .sType               = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
        .pNext               = nullptr,
        .flags               = 0,
        .stageCount          = 2,
        .pStages             = shaderStages,
        .pVertexInputState   = &vertexInputInfo,
        .pInputAssemblyState = &inputAssembly,
        .pTessellationState  = nullptr,
        .pViewportState      = &viewportState,
        .pRasterizationState = &rasterizer,
        .pMultisampleState   = &multisampling,
        .pDepthStencilState  = nullptr,
        .pColorBlendState    = &colorBlending,
        .pDynamicState       = nullptr,
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

} // namespace R3

#endif // R3_VULKAN
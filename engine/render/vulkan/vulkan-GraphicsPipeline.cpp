#include "vulkan-GraphicsPipeline.hpp"

#include <cstdint>
#include <functional>
#include <initializer_list>
#include <iterator>
#include <memory>
#include <type_traits>
#include <vector>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>
#include "api/Types.hpp"
#include "render/Shader.hpp"
#include "vulkan-Check.hpp"
#include "vulkan-Handle.hpp"
#include "vulkan-RenderContext.hpp"
#include "vulkan-ShaderMetadata.hpp"
#include "vulkan-Translation.hpp"

#include "core/Log.hpp"

namespace R3::vulkan {

GraphicsPipeline::GraphicsPipeline(RenderContext& ctx,
                                   Shader& vertexShader,
                                   Shader& fragmentShader,
                                   uint32 msaaSamples,
                                   std::initializer_list<VkFormat> colorFormats,
                                   std::initializer_list<VkPipelineColorBlendAttachmentState> colorBlends,
                                   std::initializer_list<VkDescriptorSetLayout> layouts) {
    m_device = ctx.device();

    const VkPipelineShaderStageCreateInfo shaderStagesInfo[2] = {
        {
            .sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            .stage  = TO_VK_SHADER_STAGE(vertexShader.stage()),
            .module = vertexShader.shaderHandle(),
            .pName  = "main",
        },
        {
            .sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            .stage  = TO_VK_SHADER_STAGE(fragmentShader.stage()),
            .module = fragmentShader.shaderHandle(),
            .pName  = "main",
        },
    };

    const VkPipelineVertexInputStateCreateInfo vertexInputStateInfo = {
        .sType                           = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
        .vertexBindingDescriptionCount   = (uint32_t)vertexShader.metadata().vertexInputBindingDescriptions.size(),
        .pVertexBindingDescriptions      = vertexShader.metadata().vertexInputBindingDescriptions.data(),
        .vertexAttributeDescriptionCount = (uint32_t)vertexShader.metadata().vertexInputAttributeDescriptions.size(),
        .pVertexAttributeDescriptions    = vertexShader.metadata().vertexInputAttributeDescriptions.data(),
    };

    const VkPipelineInputAssemblyStateCreateInfo inputAssemblyStateInfo = {
        .sType                  = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
        .topology               = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
        .primitiveRestartEnable = VK_FALSE,
    };

    const VkDynamicState dynamicStates[] = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR,
        VK_DYNAMIC_STATE_LINE_WIDTH,
        // VK_DYNAMIC_STATE_DEPTH_BIAS,
        // VK_DYNAMIC_STATE_BLEND_CONSTANTS,
        // VK_DYNAMIC_STATE_DEPTH_BOUNDS,
        // VK_DYNAMIC_STATE_STENCIL_COMPARE_MASK,
        // VK_DYNAMIC_STATE_STENCIL_WRITE_MASK,
        // VK_DYNAMIC_STATE_STENCIL_REFERENCE,
        VK_DYNAMIC_STATE_CULL_MODE,
        VK_DYNAMIC_STATE_FRONT_FACE,
        // VK_DYNAMIC_STATE_PRIMITIVE_TOPOLOGY,
        // VK_DYNAMIC_STATE_VIEWPORT_WITH_COUNT,
        // VK_DYNAMIC_STATE_SCISSOR_WITH_COUNT,
        // VK_DYNAMIC_STATE_VERTEX_INPUT_BINDING_STRIDE,
        VK_DYNAMIC_STATE_DEPTH_TEST_ENABLE,
        // VK_DYNAMIC_STATE_DEPTH_WRITE_ENABLE,
        // VK_DYNAMIC_STATE_DEPTH_COMPARE_OP,
        // VK_DYNAMIC_STATE_DEPTH_BOUNDS_TEST_ENABLE,
        // VK_DYNAMIC_STATE_STENCIL_TEST_ENABLE,
        // VK_DYNAMIC_STATE_STENCIL_OP,
        // VK_DYNAMIC_STATE_RASTERIZER_DISCARD_ENABLE,
        // VK_DYNAMIC_STATE_DEPTH_BIAS_ENABLE,
        // VK_DYNAMIC_STATE_PRIMITIVE_RESTART_ENABLE,
        // VK_DYNAMIC_STATE_LINE_STIPPLE,
    };

    const VkPipelineDynamicStateCreateInfo dynamicStateInfo = {
        .sType             = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
        .dynamicStateCount = static_cast<uint32_t>(std::size(dynamicStates)),
        .pDynamicStates    = dynamicStates,
    };

    const VkPipelineViewportStateCreateInfo viewportStateInfo = {
        .sType         = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
        .viewportCount = 1,
        .pViewports    = nullptr, // dynamic
        .scissorCount  = 1,
        .pScissors     = nullptr, // dynamic
    };

    const VkPipelineRasterizationStateCreateInfo rasterizationStateInfo = {
        .sType                   = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
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
        .rasterizationSamples  = (VkSampleCountFlagBits)msaaSamples,
        .sampleShadingEnable   = VK_FALSE,
        .minSampleShading      = 0.0f,
        .pSampleMask           = nullptr,
        .alphaToCoverageEnable = VK_FALSE,
        .alphaToOneEnable      = VK_FALSE,
    };

    const VkPipelineDepthStencilStateCreateInfo depthStencilStateInfo = {
        .sType                 = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
        .depthTestEnable       = VK_TRUE,
        .depthWriteEnable      = VK_TRUE,
        .depthCompareOp        = VK_COMPARE_OP_LESS_OR_EQUAL,
        .depthBoundsTestEnable = VK_FALSE,
        .stencilTestEnable     = VK_FALSE,
        .front                 = {},
        .back                  = {},
        .minDepthBounds        = 0.0f,
        .maxDepthBounds        = 1.0f,
    };

    const VkPipelineColorBlendStateCreateInfo colorBlendStateInfo = {
        .sType           = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
        .logicOpEnable   = VK_FALSE,
        .logicOp         = VK_LOGIC_OP_COPY,
        .attachmentCount = static_cast<uint32>(colorBlends.size()),
        .pAttachments    = colorBlends.begin(),
        .blendConstants  = {0.0f, 0.0f, 0.0f, 0.0f},
    };

    std::vector<VkPushConstantRange> pushConstantRanges;

    if (vertexShader.metadata().pushConstantRanges.size() != fragmentShader.metadata().pushConstantRanges.size()) {
        LOG_ERROR("mismatched push constant ranges between vertex and fragment shaders, should be identical");
    } else {
        for (usize i = 0; i < vertexShader.metadata().pushConstantRanges.size(); i++) {
            if (vertexShader.metadata().pushConstantRanges[i].size !=
                fragmentShader.metadata().pushConstantRanges[i].size) {
                LOG_ERROR("mismatched push constant ranges between vertex and fragment shaders, should be identical");
            } else {
                pushConstantRanges.push_back(vertexShader.metadata().pushConstantRanges[i]);
                pushConstantRanges.back().stageFlags |= fragmentShader.metadata().pushConstantRanges[i].stageFlags;
            }
        }
    }

    const VkPipelineLayoutCreateInfo pipelineLayoutInfo = {
        .sType                  = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .setLayoutCount         = static_cast<uint32>(layouts.size()),
        .pSetLayouts            = layouts.begin(),
        .pushConstantRangeCount = static_cast<uint32>(pushConstantRanges.size()),
        .pPushConstantRanges    = pushConstantRanges.data(),
    };
    VK_CHECK(vkCreatePipelineLayout(m_device, &pipelineLayoutInfo, nullptr, &*m_pipelineLayout));

    const VkPipelineRenderingCreateInfo renderingInfo = {
        .sType                   = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO,
        .viewMask                = 0,
        .colorAttachmentCount    = static_cast<uint32>(colorFormats.size()),
        .pColorAttachmentFormats = colorFormats.begin(),
        .depthAttachmentFormat   = ctx.queryDepthFormat(),
        .stencilAttachmentFormat = VK_FORMAT_UNDEFINED,
    };

    const VkGraphicsPipelineCreateInfo graphicsPipelineInfo = {
        .sType               = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
        .pNext               = &renderingInfo,
        .stageCount          = static_cast<uint32>(std::size(shaderStagesInfo)),
        .pStages             = shaderStagesInfo,
        .pVertexInputState   = &vertexInputStateInfo,
        .pInputAssemblyState = &inputAssemblyStateInfo,
        .pTessellationState  = nullptr,
        .pViewportState      = &viewportStateInfo,
        .pRasterizationState = &rasterizationStateInfo,
        .pMultisampleState   = &multisampeStateInfo,
        .pDepthStencilState  = &depthStencilStateInfo,
        .pColorBlendState    = &colorBlendStateInfo,
        .pDynamicState       = &dynamicStateInfo,
        .layout              = m_pipelineLayout,
        .renderPass          = VK_NULL_HANDLE,
        .subpass             = 0,
        .basePipelineHandle  = VK_NULL_HANDLE,
        .basePipelineIndex   = -1,
    };

    try {
        VK_CHECK(vkCreateGraphicsPipelines(m_device, VK_NULL_HANDLE, 1, &graphicsPipelineInfo, nullptr, &*m_pipeline));
    } catch (...) {
        vkDestroyPipelineLayout(m_device, m_pipelineLayout, nullptr);
        throw;
    }
}

GraphicsPipeline::~GraphicsPipeline() noexcept {
    if (m_device) {
        vkDestroyPipeline(m_device, m_pipeline, nullptr);
        vkDestroyPipelineLayout(m_device, m_pipelineLayout, nullptr);
    }
}

} // namespace R3::vulkan

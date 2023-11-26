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
    CHECK(spec.pipelineLayout != nullptr);
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
        .width = static_cast<float>(m_spec.swapchain->extent().x),
        .height = static_cast<float>(m_spec.swapchain->extent().y),
        .minDepth = 0.0f,
        .maxDepth = 1.0f,
    };

    VkRect2D scissor = {
        .offset = {0, 0},
        .extent = {m_spec.swapchain->extent().x, m_spec.swapchain->extent().y},
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

    VkPipelineViewportStateCreateInfo viewportStateCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0U,
        .viewportCount = 1,
    #if STATIC_STATE
        .pViewports = &viewport,
    #endif
        .scissorCount = 1,
    #if STATIC_STATE
        .pScissors = &scissor,
    #endif
    };

    VkPipelineRasterizationStateCreateInfo rasterizationStateCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0U,
        .depthClampEnable = VK_FALSE,
        .rasterizerDiscardEnable = VK_FALSE,
        .polygonMode = VK_POLYGON_MODE_FILL,
        .cullMode = VK_CULL_MODE_BACK_BIT,
        .frontFace = VK_FRONT_FACE_CLOCKWISE,
        .depthBiasEnable = VK_FALSE,
        .depthBiasConstantFactor = 0.0f,
        .depthBiasClamp = 0.0f,
        .depthBiasSlopeFactor = 0.0f,
        .lineWidth = 1.0f,
    };

    VkPipelineMultisampleStateCreateInfo multisampleStateCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0U,
        .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
        .sampleShadingEnable = VK_FALSE,
        .minSampleShading = 1.0f,
        .pSampleMask = nullptr,
        .alphaToCoverageEnable = VK_FALSE,
        .alphaToOneEnable = VK_FALSE,
    };

    #if TODO
    VkPipelineDepthStencilStateCreateInfo depthStencilCreateInfo = {
        .sType = ,
        .pNext = ,
        .flags = ,
        .depthTestEnable = ,
        .depthWriteEnable = ,
        .depthCompareOp = ,
        .depthBoundsTestEnable = ,
        .stencilTestEnable = ,
        .front = ,
        .back = ,
        .minDepthBounds = ,
        .maxDepthBounds = ,
    };
    #endif

    VkPipelineColorBlendAttachmentState colorBlendAttachmentState = {
        .blendEnable = VK_FALSE,
        .srcColorBlendFactor = VK_BLEND_FACTOR_ONE,
        .dstColorBlendFactor = VK_BLEND_FACTOR_ZERO,
        .colorBlendOp = VK_BLEND_OP_ADD,
        .srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
        .dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
        .alphaBlendOp = VK_BLEND_OP_ADD,
        .colorWriteMask =
            VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
    };

    VkPipelineColorBlendStateCreateInfo colorBlendStateCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0U,
        .logicOpEnable = VK_FALSE,
        .logicOp = VK_LOGIC_OP_COPY,
        .attachmentCount = 1,
        .pAttachments = &colorBlendAttachmentState,
        .blendConstants =
            {
                0.0f,
                0.0f,
                0.0f,
                0.0f,
            },
    };
}

void GraphicsPipeline::destroy() {
    m_vertexShader.destroy();
    m_fragmentShader.destroy();
}

} // namespace R3

#endif // R3_VULKAN

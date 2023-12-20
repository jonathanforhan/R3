#if R3_VULKAN

#include "render/GraphicsPipeline.hpp"

#include <vulkan/vulkan.hpp>
#include "api/Check.hpp"
#include "api/Ensure.hpp"
#include "render/LogicalDevice.hpp"
#include "render/Mesh.hpp"
#include "render/PipelineLayout.hpp"
#include "render/RenderPass.hpp"
#include "render/Swapchain.hpp"

namespace R3 {

GraphicsPipeline::GraphicsPipeline(const GraphicsPipelineSpecification& spec)
    : m_spec(spec) {
    m_layout = PipelineLayout({
        .logicalDevice = m_spec.logicalDevice,
        .descriptorSetLayout = m_spec.descriptorSetLayout,
    });

    m_vertexShader = Shader({.logicalDevice = m_spec.logicalDevice, .path = m_spec.vertexShaderPath});
    m_fragmentShader = Shader({.logicalDevice = m_spec.logicalDevice, .path = m_spec.fragmentShaderPath});

    const vk::PipelineShaderStageCreateInfo vertexShaderStageCreateInfo = {
        .sType = vk::StructureType::ePipelineShaderStageCreateInfo,
        .pNext = nullptr,
        .flags = {},
        .stage = vk::ShaderStageFlagBits::eVertex,
        .module = m_vertexShader.as<vk::ShaderModule>(),
        .pName = "main",
        .pSpecializationInfo = nullptr,
    };

    const vk::PipelineShaderStageCreateInfo fragmentShaderStageCreateInfo = {
        .sType = vk::StructureType::ePipelineShaderStageCreateInfo,
        .pNext = nullptr,
        .flags = {},
        .stage = vk::ShaderStageFlagBits::eFragment,
        .module = m_fragmentShader.as<vk::ShaderModule>(),
        .pName = "main",
        .pSpecializationInfo = nullptr,
    };

    const vk::PipelineShaderStageCreateInfo shaderStageCreateInfos[] = {
        vertexShaderStageCreateInfo,
        fragmentShaderStageCreateInfo,
    };

    const vk::VertexInputBindingDescription bindingDescription = {
        .binding = 0,
        .stride = sizeof(Vertex),
        .inputRate = vk::VertexInputRate::eVertex,
    };

    const vk::VertexInputAttributeDescription attributeDescriptions[] = {
        {
            .location = 0,
            .binding = 0,
            .format = vk::Format::eR32G32B32Sfloat,
            .offset = offsetof(Vertex, position),
        },
        {
            .location = 1,
            .binding = 0,
            .format = vk::Format::eR32G32B32Sfloat,
            .offset = offsetof(Vertex, normal),
        },
        {
            .location = 2,
            .binding = 0,
            .format = vk::Format::eR32G32B32Sfloat,
            .offset = offsetof(Vertex, tangent),
        },
        {
            .location = 3,
            .binding = 0,
            .format = vk::Format::eR32G32B32Sfloat,
            .offset = offsetof(Vertex, tangent),
        },
        {
            .location = 4,
            .binding = 0,
            .format = vk::Format::eR32G32Sfloat,
            .offset = offsetof(Vertex, textureCoords),
        },
#if TODO_ANIMATION
        {
            .location = 5,
            .binding = 0,
            .format = VK_FORMAT_R32_SINT,
            .offset = offsetof(Vertex, boneIDs),
        },
        {
            .location = 6,
            .binding = 0,
            .format = VK_FORMAT_R32_SFLOAT,
            .offset = offsetof(Vertex, weights),
        },
#endif
    };

    const vk::PipelineVertexInputStateCreateInfo vertexInputStateCreateInfo = {
        .sType = vk::StructureType::ePipelineVertexInputStateCreateInfo,
        .pNext = nullptr,
        .flags = {},
        .vertexBindingDescriptionCount = 1,
        .pVertexBindingDescriptions = &bindingDescription,
        .vertexAttributeDescriptionCount = sizeof(attributeDescriptions) / sizeof(*attributeDescriptions),
        .pVertexAttributeDescriptions = attributeDescriptions,
    };

    const vk::PipelineInputAssemblyStateCreateInfo inputAssemblyStateCreateInfo = {
        .sType = vk::StructureType::ePipelineInputAssemblyStateCreateInfo,
        .pNext = nullptr,
        .flags = {},
        .topology = vk::PrimitiveTopology::eTriangleList,
        .primitiveRestartEnable = vk::False,
    };

    const std::vector<vk::DynamicState> dynamicStates = {
        vk::DynamicState::eViewport,
        vk::DynamicState::eScissor,
    };

    const vk::PipelineDynamicStateCreateInfo dynamicStateCreateInfo = {
        .sType = vk::StructureType::ePipelineDynamicStateCreateInfo,
        .pNext = nullptr,
        .flags = {},
        .dynamicStateCount = static_cast<uint32>(dynamicStates.size()),
        .pDynamicStates = dynamicStates.data(),
    };

    const vk::PipelineViewportStateCreateInfo viewportStateCreateInfo = {
        .sType = vk::StructureType::ePipelineViewportStateCreateInfo,
        .pNext = nullptr,
        .flags = {},
        .viewportCount = 1,
        .pViewports = nullptr,
        .scissorCount = 1,
        .pScissors = nullptr,
    };

    const vk::PipelineRasterizationStateCreateInfo rasterizationStateCreateInfo = {
        .sType = vk::StructureType::ePipelineRasterizationStateCreateInfo,
        .pNext = nullptr,
        .flags = {},
        .depthClampEnable = vk::False,
        .rasterizerDiscardEnable = vk::False,
        .polygonMode = vk::PolygonMode::eFill,
        .cullMode = vk::CullModeFlagBits::eBack,
        .frontFace = vk::FrontFace::eCounterClockwise,
        .depthBiasEnable = vk::False,
        .depthBiasConstantFactor = 0.0f,
        .depthBiasClamp = 0.0f,
        .depthBiasSlopeFactor = 0.0f,
        .lineWidth = 1.0f,
    };

    const vk::PipelineMultisampleStateCreateInfo multisampleStateCreateInfo = {
        .sType = vk::StructureType::ePipelineMultisampleStateCreateInfo,
        .pNext = nullptr,
        .flags = {},
        .rasterizationSamples = vk::SampleCountFlagBits::e1,
        .sampleShadingEnable = vk::False,
        .minSampleShading = 1.0f,
        .pSampleMask = nullptr,
        .alphaToCoverageEnable = vk::False,
        .alphaToOneEnable = vk::False,
    };

    vk::PipelineDepthStencilStateCreateInfo depthStencilCreateInfo = {
        .sType = vk::StructureType::ePipelineDepthStencilStateCreateInfo,
        .pNext = nullptr,
        .flags = {},
        .depthTestEnable = vk::True,
        .depthWriteEnable = vk::True,
        .depthCompareOp = vk::CompareOp::eLess,
        .depthBoundsTestEnable = vk::False,
        .stencilTestEnable = vk::False,
        .front = {},
        .back = {},
        .minDepthBounds = 0.0f,
        .maxDepthBounds = 1.0f,
    };

    const vk::PipelineColorBlendAttachmentState colorBlendAttachmentState = {
        .blendEnable = vk::False,
        .srcColorBlendFactor = vk::BlendFactor::eOne,
        .dstColorBlendFactor = vk::BlendFactor::eZero,
        .colorBlendOp = vk::BlendOp::eAdd,
        .srcAlphaBlendFactor = vk::BlendFactor::eOne,
        .dstAlphaBlendFactor = vk::BlendFactor::eZero,
        .alphaBlendOp = vk::BlendOp::eAdd,
        .colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG |
                          vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA,
    };

    const vk::PipelineColorBlendStateCreateInfo colorBlendStateCreateInfo = {
        .sType = vk::StructureType::ePipelineColorBlendStateCreateInfo,
        .pNext = nullptr,
        .flags = {},
        .logicOpEnable = VK_FALSE,
        .logicOp = vk::LogicOp::eCopy,
        .attachmentCount = 1,
        .pAttachments = &colorBlendAttachmentState,
        .blendConstants = {{
            0.0f,
            0.0f,
            0.0f,
            0.0f,
        }},
    };

    const vk::GraphicsPipelineCreateInfo graphicsPipelineCreateInfo = {
        .sType = vk::StructureType::eGraphicsPipelineCreateInfo,
        .pNext = nullptr,
        .flags = {},
        .stageCount = 2,
        .pStages = shaderStageCreateInfos,
        .pVertexInputState = &vertexInputStateCreateInfo,
        .pInputAssemblyState = &inputAssemblyStateCreateInfo,
        .pTessellationState = nullptr,
        .pViewportState = &viewportStateCreateInfo,
        .pRasterizationState = &rasterizationStateCreateInfo,
        .pMultisampleState = &multisampleStateCreateInfo,
        .pDepthStencilState = &depthStencilCreateInfo,
        .pColorBlendState = &colorBlendStateCreateInfo,
        .pDynamicState = &dynamicStateCreateInfo,
        .layout = m_layout.as<vk::PipelineLayout>(),
        .renderPass = m_spec.renderPass->as<vk::RenderPass>(),
        .subpass = 0,
        .basePipelineHandle = VK_NULL_HANDLE,
        .basePipelineIndex = -1,
    };

    const auto r = m_spec.logicalDevice->as<vk::Device>().createGraphicsPipeline(nullptr, graphicsPipelineCreateInfo);
    ENSURE(r.result == vk::Result::eSuccess);
    setHandle(r.value);
}

GraphicsPipeline::~GraphicsPipeline() {
    if (validHandle()) {
        m_spec.logicalDevice->as<vk::Device>().destroyPipeline(as<vk::Pipeline>());
    }
}

} // namespace R3

#endif // R3_VULKAN

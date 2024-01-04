#if R3_VULKAN

#include "render/GraphicsPipeline.hpp"

#include <vulkan/vulkan.hpp>
#include "api/Ensure.hpp"
#include "render/LogicalDevice.hpp"
#include "render/PhysicalDevice.hpp"
#include "render/PipelineLayout.hpp"
#include "render/RenderPass.hpp"
#include "render/Swapchain.hpp"
#include "render/Vertex.hpp"

namespace R3 {

GraphicsPipeline::GraphicsPipeline(const GraphicsPipelineSpecification& spec)
    : m_logicalDevice(&spec.logicalDevice) {
    m_layout = PipelineLayout({
        .logicalDevice = spec.logicalDevice,
        .descriptorSetLayout = spec.descriptorSetLayout,
    });

    m_vertexShader = Shader({
        .logicalDevice = spec.logicalDevice,
        .path = spec.vertexShaderPath,
    });

    m_fragmentShader = Shader({
        .logicalDevice = spec.logicalDevice,
        .path = spec.fragmentShaderPath,
    });

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
        .binding = spec.vertexBindingSpecification.binding,
        .stride = spec.vertexBindingSpecification.stride,
        .inputRate = vk::VertexInputRate(spec.vertexBindingSpecification.inputRate),
    };

    // transform R3 attribute to vk descriptor
    auto vertexAttributeToDescription = [](const VertexAttributeSpecification& attribute) {
        return vk::VertexInputAttributeDescription{
            .location = attribute.location,
            .binding = attribute.binding,
            .format = vk::Format(attribute.format),
            .offset = attribute.offset,
        };
    };
    std::vector<vk::VertexInputAttributeDescription> attributeDescriptions(spec.vertexAttributeSpecification.size());
    std::ranges::transform(
        spec.vertexAttributeSpecification, attributeDescriptions.begin(), vertexAttributeToDescription);

    const vk::PipelineVertexInputStateCreateInfo vertexInputStateCreateInfo = {
        .sType = vk::StructureType::ePipelineVertexInputStateCreateInfo,
        .pNext = nullptr,
        .flags = {},
        .vertexBindingDescriptionCount = 1,
        .pVertexBindingDescriptions = &bindingDescription,
        .vertexAttributeDescriptionCount = static_cast<uint32>(std::size(attributeDescriptions)),
        .pVertexAttributeDescriptions = attributeDescriptions.data(),
    };

    const vk::PipelineInputAssemblyStateCreateInfo inputAssemblyStateCreateInfo = {
        .sType = vk::StructureType::ePipelineInputAssemblyStateCreateInfo,
        .pNext = nullptr,
        .flags = {},
        .topology = vk::PrimitiveTopology::eTriangleList,
        .primitiveRestartEnable = vk::False,
    };

    // we then bind these in the command buffer when we bind our pipeline
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
        .rasterizationSamples = vk::SampleCountFlagBits(spec.msaa ? spec.physicalDevice.sampleCount() : 1),
        .sampleShadingEnable = spec.msaa ? vk::True : vk::False,
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
        .logicOpEnable = vk::False,
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
        .renderPass = spec.renderPass.as<vk::RenderPass>(),
        .subpass = 0,
        .basePipelineHandle = VK_NULL_HANDLE,
        .basePipelineIndex = -1,
    };

    const auto r = m_logicalDevice->as<vk::Device>().createGraphicsPipeline(nullptr, graphicsPipelineCreateInfo);
    ENSURE(r.result == vk::Result::eSuccess);
    setHandle(r.value);
}

GraphicsPipeline::~GraphicsPipeline() {
    if (validHandle()) {
        m_logicalDevice->as<vk::Device>().destroyPipeline(as<vk::Pipeline>());
    }
}

} // namespace R3

#endif // R3_VULKAN

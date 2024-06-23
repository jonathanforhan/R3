#pragma once

#if R3_VULKAN

#include "vulkan-fwd.hxx"
#include "vulkan-ShaderModule.hxx"
#include "vulkan-VulkanObject.hxx"
#include <api/Construct.hpp>
#include <optional>
#include <span>
#include <vulkan/vulkan.h>

namespace R3::vulkan {

// takes ownership of shader modules passed to it and will clean them up
// when pipeline dies
struct GraphicsPipelineSpecification {
    const PhysicalDevice& physicalDevice;
    const LogicalDevice& device;
    const Swapchain& swapchain;
    const RenderPass& renderPass;
    VkDescriptorSetLayout descriptorSetLayout;
    VkVertexInputBindingDescription bindingDescription;
    std::span<VkVertexInputAttributeDescription> attributeDescriptions;
    ShaderModule&& vertexShader;
    ShaderModule&& fragmentShader;
    bool msaa;
};

class GraphicsPipeline : public VulkanObject<VkPipeline>, public VulkanObject<VkPipelineLayout> {
public:
    using VulkanObject<VkPipeline>::vk;

public:
    DEFAULT_CONSTRUCT(GraphicsPipeline);
    NO_COPY(GraphicsPipeline);
    DEFAULT_MOVE(GraphicsPipeline);

    GraphicsPipeline(const GraphicsPipelineSpecification& spec);

    ~GraphicsPipeline();

    constexpr VkPipelineLayout layout() const { return VulkanObject<VkPipelineLayout>::vk(); }

private:
    VkDevice m_device = VK_NULL_HANDLE;
    ShaderModule m_vertexShader;
    ShaderModule m_fragmentShader;
};

} // namespace R3::vulkan

#endif // R3_VULKAN
/**
 * @file vulkan-GraphicsPipeline.hxx
 * @copyright GNU Public License
 *
 * Graphics Pipeline Overview
 *
 * [Vertex/Index buffer]
 *     |
 *     V
 * [Vertex Shader]
 *     |
 *     V
 * [Tessellation]
 *     |
 *     V
 * [Geometry Shader]
 *     |
 *     V
 * [Rasterization]
 *     |
 *     V
 * [Fragment Shader]
 *     |
 *     V
 * [Color Blending]
 *     |
 *     V
 * [Framebuffer]
 */

#pragma once

#if R3_VULKAN

#include <vulkan/vulkan.h>
#include <api/Construct.hpp>
#include <span>
#include "vulkan-ShaderModule.hxx"
#include "vulkan-VulkanObject.hxx"
#include "vulkan-fwd.hxx"

namespace R3::vulkan {

/**
 * Vulkan GraphicsPipeline Specification.
 * ShaderModules must be moved as r-value references because the
 * GraphicsPipeline will take ownership of them. There is no need to manually
 * track the shaders passed in as they will be invalidated on move.
 * @note DO NOT use ShaderModules after they are moved
 */
struct GraphicsPipelineSpecification {
    const PhysicalDevice& physicalDevice;                               /**< Valid PhysicalDevice. */
    const LogicalDevice& device;                                        /**< Valid LogicalDevice. */
    const Swapchain& swapchain;                                         /**< Valid Swapchain. */
    const RenderPass& renderPass;                                       /**< Valid RenderPass. */
    VkDescriptorSetLayout descriptorSetLayout;                          /**< DescriptorSetLayout for PipelineLayout. */
    std::span<VkVertexInputBindingDescription> bindingDescriptions;     /**< Vertex Bindings, MAY be empty. */
    std::span<VkVertexInputAttributeDescription> attributeDescriptions; /**< Vertex Attributes, MAY be empty. */
    ShaderModule&& vertexShader;                                        /**< Moved ShaderModule. */
    ShaderModule&& fragmentShader;                                      /**< Moved ShaderModule. */
    bool msaa;                                                          /**< Enable MSAA. */
};

/**
 * Vulkan GraphicsPipeline RAII wrapper.
 * GraphicsPipeline takes ownership of all ShaderModules passed to it during
 * creation and will free them in the GraphicsPipeline destructor.
 * https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/VkPipeline.html
 * https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/VkPipelineLayout.html
 */
class GraphicsPipeline : public VulkanObject<VkPipeline>, public VulkanObject<VkPipelineLayout> {
public:
    using VulkanObject<VkPipeline>::vk; /**< Uses VkPipeline as main parent. */

public:
    DEFAULT_CONSTRUCT(GraphicsPipeline);
    NO_COPY(GraphicsPipeline);
    DEFAULT_MOVE(GraphicsPipeline);

    /**
     * Construct GraphicsPipeline.
     * On construction GraphicsPipeline creates the VkPipelineLayout, and the
     * VkPipeline. The Viewport and Scissor are created dynamically so they must
     * be set every time the CommandBuffer is recorded.
     * @param spec
     * @note GraphicsPipeline takes ownership of ShaderModules passed to it
     */
    GraphicsPipeline(const GraphicsPipelineSpecification& spec);

    /**
     * Destroys GraphicsPipeline, and ShaderModules.
     */
    ~GraphicsPipeline();

    /**
     * @return PipelineLayout
     */
    constexpr VkPipelineLayout layout() const { return VulkanObject<VkPipelineLayout>::vk(); }

private:
    VkDevice m_device = VK_NULL_HANDLE;
    ShaderModule m_vertexShader;
    ShaderModule m_fragmentShader;
};

} // namespace R3::vulkan

#endif // R3_VULKAN
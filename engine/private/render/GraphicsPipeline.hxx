#pragma once

/// GraphicsPipeline Describes the all stages of the Render Pipeline

#include <span>
#include <string_view>
#include "render/PipelineLayout.hxx"
#include "render/RenderApi.hxx"
#include "render/Shader.hxx"
#include "render/Vertex.hxx"

namespace R3 {

/// @brief Graphics Pipeline Specification
struct GraphicsPipelineSpecification {
    const PhysicalDevice& physicalDevice;
    const LogicalDevice& logicalDevice;
    const Swapchain& swapchain;
    const RenderPass& renderPass;
    const DescriptorSetLayout& descriptorSetLayout;
    const VertexBindingSpecification& vertexBindingSpecification;
    std::span<const VertexAttributeSpecification> vertexAttributeSpecification;
    std::string_view vertexShaderPath;
    std::string_view fragmentShaderPath;
    bool msaa; ///< Multisample enable
};

/// @brief GraphicsPipeline created Pipeline from DescriptorLayout and genrates Shader Modules
/// Pipelines are allocated by GlobalResourceManager
class GraphicsPipeline : public NativeRenderObject {
public:
    DEFAULT_CONSTRUCT(GraphicsPipeline);
    NO_COPY(GraphicsPipeline);
    DEFAULT_MOVE(GraphicsPipeline);

    /// @brief Construct Graphics Pipeline from spec
    /// @param spec
    GraphicsPipeline(const GraphicsPipelineSpecification& spec);

    /// @brief Destroy Pipeline
    ~GraphicsPipeline();

    /// @brief Query PipelineLayout
    /// @return Layout
    [[nodiscard]] constexpr const PipelineLayout& layout() const { return m_layout; }

private:
    Ref<const LogicalDevice> m_logicalDevice;
    PipelineLayout m_layout;
    Shader m_vertexShader;
    Shader m_fragmentShader;
};

} // namespace R3

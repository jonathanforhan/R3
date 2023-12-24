#pragma once

/// @file GraphicsPipeline.hpp
/// GraphicsPipeline Describes the all stages of the Render Pipeline

#include <string_view>
#include "render/PipelineLayout.hpp"
#include "render/RenderApi.hpp"
#include "render/Shader.hpp"

namespace R3 {

/// @brief Graphics Pipeline Specification
struct GraphicsPipelineSpecification {
    const PhysicalDevice& physicalDevice;           ///< PhysicalDevice
    const LogicalDevice& logicalDevice;             ///< LogicalDevice
    const Swapchain& swapchain;                     ///< Swapchain
    const RenderPass& renderPass;                   ///< RenderPass
    const DescriptorSetLayout& descriptorSetLayout; ///< DescriptorSetLayout
    std::string_view vertexShaderPath;              ///< Vertex Shader filepath
    std::string_view fragmentShaderPath;            ///< Fragment Shader filepath
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
    const PipelineLayout& layout() const { return m_layout; }

private:
    Ref<const LogicalDevice> m_logicalDevice;
    PipelineLayout m_layout;
    Shader m_vertexShader;
    Shader m_fragmentShader;
};

} // namespace R3

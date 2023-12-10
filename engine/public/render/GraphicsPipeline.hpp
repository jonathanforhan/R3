#pragma once

#include <string_view>
#include "render/NativeRenderObject.hpp"
#include "render/RenderFwd.hpp"
#include "render/Shader.hpp"

namespace R3 {

struct GraphicsPipelineSpecification {
    Ref<const LogicalDevice> logicalDevice;
    Ref<const Swapchain> swapchain;
    Ref<const RenderPass> renderPass;
    Ref<const PipelineLayout> pipelineLayout;
    std::string_view vertexShaderPath;
    std::string_view fragmentShaderPath;
};

class GraphicsPipeline : public NativeRenderObject {
public:
    GraphicsPipeline() = default;
    GraphicsPipeline(const GraphicsPipelineSpecification& spec);
    GraphicsPipeline(GraphicsPipeline&&) noexcept = default;
    GraphicsPipeline& operator=(GraphicsPipeline&&) noexcept = default;
    ~GraphicsPipeline();

private:
    GraphicsPipelineSpecification m_spec;
    Shader m_vertexShader;
    Shader m_fragmentShader;
};

} // namespace R3

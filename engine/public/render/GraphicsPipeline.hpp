#pragma once
#include <string_view>
#include "render/NativeRenderObject.hpp"
#include "render/RenderFwd.hpp"
#include "render/Shader.hpp"

namespace R3 {

struct GraphicsPipelineSpecification {
    const LogicalDevice* logicalDevice;
    const Swapchain* swapchain;
    const RenderPass* renderPass;
    const PipelineLayout* pipelineLayout;
    std::string_view vertexShaderPath;
    std::string_view fragmentShaderPath;
};

class GraphicsPipeline : public NativeRenderObject {
public:
    void create(const GraphicsPipelineSpecification& spec);
    void destroy();

private:
    GraphicsPipelineSpecification m_spec;
    Shader m_vertexShader;
    Shader m_fragmentShader;
};

} // namespace R3

#pragma once

#include <span>
#include <volk.h>
#include "api/Class.hpp"
#include "vulkan-Fwd.hpp"
#include "vulkan-Handle.hpp"

namespace R3::vulkan {

class GraphicsPipeline {
public:
    R3_CTOR_DEFAULT(GraphicsPipeline);
    R3_COPY_DELETE(GraphicsPipeline);
    R3_MOVE_DEFAULT(GraphicsPipeline);

    GraphicsPipeline(RenderContext& ctx,
                     Shader& vertexShader,
                     Shader& fragmentShader,
                     VkSampleCountFlagBits msaaSamples,
                     std::span<const VkFormat> colorFormats,
                     std::span<const VkDescriptorSetLayout> layouts);

    ~GraphicsPipeline() noexcept;

    VkPipeline pipeline() const noexcept { return m_pipeline; }

    VkPipelineLayout layout() const noexcept { return m_pipelineLayout; }

private:
    Handle<VkDevice> m_device;
    Handle<VkPipeline> m_pipeline;
    Handle<VkPipelineLayout> m_pipelineLayout;
};

} // namespace R3::vulkan
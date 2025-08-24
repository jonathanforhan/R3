#pragma once

#include <span>
#include <vector>
#include <vulkan/vulkan_core.h>
#include "api/Class.hpp"
#include "api/Types.hpp"
#include "vulkan-Handle.hpp"
#include "vulkan-RenderContext.hpp"
#include "vulkan-RenderPass.hpp"
#include "vulkan-Shader.hpp"

namespace R3::vulkan {

struct Vertex {
    fvec3 position; // x, y, z
    fvec3 color;    // r, g, b
    fvec2 texCoord; // u, v

    static VkVertexInputBindingDescription getBindingDescription() noexcept;
    static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions() noexcept;
};

class GraphicsPipeline {
public:
    R3_CTOR_DEFAULT(GraphicsPipeline);
    R3_COPY_DELETE(GraphicsPipeline);
    R3_MOVE_DEFAULT(GraphicsPipeline);

    GraphicsPipeline(RenderContext& ctx,
                     RenderPass& renderPass,
                     Shader& vertexShader,
                     Shader& fragmentShader,
                     VkSampleCountFlagBits msaaSamples,
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
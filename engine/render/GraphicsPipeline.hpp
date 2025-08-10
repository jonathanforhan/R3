#pragma once

#include <span>
#include <vector>
#include <vulkan/vulkan_core.h>
#include "Types.hpp"

namespace R3 {

class RenderContext;
class RenderPass;
class Shader;

struct Vertex {
    fvec3 position; // x, y, z
    fvec3 color;    // r, g, b

    static VkVertexInputBindingDescription getBindingDescription() noexcept;
    static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions() noexcept;
};

class GraphicsPipeline {
public:
    void create(RenderContext& ctx,
                RenderPass& renderPass,
                Shader& vertexShader,
                Shader& fragmentShader,
                uvec2 extent,
                std::span<const VkDescriptorSetLayout> layouts);

    void destroy() noexcept;

    void bind(VkCommandBuffer commandBuffer) const;

    VkPipeline handle() noexcept { return m_pipeline; }

    VkPipelineLayout layout() noexcept { return m_pipelineLayout; }

    bool isValid() const noexcept { return m_pipeline != VK_NULL_HANDLE; }

private:
    VkDevice m_device                 = VK_NULL_HANDLE;
    VkPipeline m_pipeline             = VK_NULL_HANDLE;
    VkPipelineLayout m_pipelineLayout = VK_NULL_HANDLE;
};

} // namespace R3
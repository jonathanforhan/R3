#pragma once

#include <span>
#include <vector>
#include <vulkan/vulkan_core.h>
#include "Types.hpp"
#include "vulkan-RenderContext.hpp"
#include "vulkan-RenderPass.hpp"
#include "vulkan-Shader.hpp"

namespace R3::vulkan {

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
                std::span<const VkDescriptorSetLayout> layouts);

    void destroy() noexcept;

    void bind(VkCommandBuffer commandBuffer) const;

    void setViewport(VkCommandBuffer cmd, const VkViewport& viewport);

    void setScissor(VkCommandBuffer cmd, const VkRect2D& scissor);

    void setCullMode(VkCommandBuffer cmd, VkCullModeFlags cullMode);

    void setFrontFace(VkCommandBuffer cmd, VkFrontFace frontFace);

    void setLineWidth(VkCommandBuffer cmd, float lineWidth);

    VkPipeline pipeline() const noexcept { return m_pipeline; }

    VkPipelineLayout layout() const noexcept { return m_pipelineLayout; }

private:
    VkDevice m_device                 = VK_NULL_HANDLE;
    VkPipeline m_pipeline             = VK_NULL_HANDLE;
    VkPipelineLayout m_pipelineLayout = VK_NULL_HANDLE;
};

} // namespace R3::vulkan
#pragma once

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

    static VkVertexInputBindingDescription getBindingDescription();
    static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();
};

class GraphicsPipeline {
public:
    void create(RenderContext& ctx,
                RenderPass& renderPass,
                Shader& vertexShader,
                Shader& fragmentShader,
                VkExtent2D extent) noexcept(false);

    void destroy() noexcept(true);

    void bind(VkCommandBuffer commandBuffer) const;

    VkPipeline handle() const { return m_pipeline; }

    VkPipelineLayout layout() const { return m_pipelineLayout; }

    bool isValid() const { return m_pipeline != VK_NULL_HANDLE; }

private:
    VkDevice m_device                 = VK_NULL_HANDLE;
    VkPipeline m_pipeline             = VK_NULL_HANDLE;
    VkPipelineLayout m_pipelineLayout = VK_NULL_HANDLE;
};

} // namespace R3
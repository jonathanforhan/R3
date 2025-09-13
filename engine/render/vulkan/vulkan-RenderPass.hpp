#pragma once

#include <cstdint>
#include <optional>
#include <vector>
#include <vulkan/vulkan.h>
#include "engine/api/Class.hpp"
#include "engine/api/Types.hpp"
#include "vulkan-Fwd.hpp"

namespace R3::vulkan {

class RenderPass {
protected:
    R3_CTOR_DEFAULT(RenderPass);
    R3_COPY_DELETE(RenderPass);
    R3_MOVE_DEFAULT(RenderPass);

public:
    virtual ~RenderPass() noexcept = default;

public:
    void execute(CommandBuffer& cmd);

    void addColorAttachment(const VkRenderingAttachmentInfo& colorAttachment) {
        m_colorAttachments.push_back(colorAttachment);
    }

    void addMemoryBarrier(const VkMemoryBarrier2& barrier) { m_barriers.push_back(barrier); }

    void addImageMemoryBarrier(const VkImageMemoryBarrier2& barrier) { m_imageBarriers.push_back(barrier); }

    void setDepthAttachment(const VkRenderingAttachmentInfo& depthAttachment) { m_depthAttachment = depthAttachment; }

    void setRenderArea(const VkRect2D& renderArea) noexcept { m_renderArea = renderArea; }

    void setGraphicsPipeline(GraphicsPipeline* pipeline) noexcept { m_pipeline = pipeline; }

    void setDescriptorSet(VkDescriptorSet descriptorSet) noexcept { m_descriptorSet = descriptorSet; }

protected:
    virtual void setDynamicPipelineStates(CommandBuffer& cmd);

    virtual void render(CommandBuffer& cmd) = 0;

protected:
    std::vector<VkMemoryBarrier2> m_barriers;
    std::vector<VkImageMemoryBarrier2> m_imageBarriers;
    std::vector<VkRenderingAttachmentInfo> m_colorAttachments;
    std::optional<VkRenderingAttachmentInfo> m_depthAttachment;
    VkRect2D m_renderArea           = {};
    GraphicsPipeline* m_pipeline    = nullptr;
    VkDescriptorSet m_descriptorSet = VK_NULL_HANDLE;
    uint32 m_imageIndex             = UINT32_MAX;
};

} // namespace R3::vulkan
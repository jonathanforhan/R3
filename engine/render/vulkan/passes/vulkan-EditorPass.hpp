#pragma once

#include <optional>
#include <vector>
#include <vulkan/vulkan.h>
#include "engine/api/Class.hpp"
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

    virtual void setDynamicPipelineStates(CommandBuffer& cmd) = 0;

    void addColorAttachment(const VkRenderingAttachmentInfo& colorAttachment) {
        m_colorAttachments.push_back(colorAttachment);
    }

    void addImageMemoryBarrier(const VkImageMemoryBarrier2& barrier) { m_barriers.push_back(barrier); }

    void setDepthAttachment(const VkRenderingAttachmentInfo& depthAttachment) { m_depthAttachment = depthAttachment; }

    void setRenderArea(const VkRect2D& renderArea) { m_renderArea = renderArea; }

protected:
    virtual void render(CommandBuffer& cmd) = 0;

protected:
    std::vector<VkImageMemoryBarrier2> m_barriers;
    std::vector<VkRenderingAttachmentInfo> m_colorAttachments;
    std::optional<VkRenderingAttachmentInfo> m_depthAttachment;
    VkRect2D m_renderArea = {};
};

} // namespace R3::vulkan

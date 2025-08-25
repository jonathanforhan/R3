#pragma once

#include <vector>
#include <vulkan/vulkan_core.h>
#include "api/Class.hpp"
#include "vulkan-Handle.hpp"
#include "vulkan-Fwd.hpp"

namespace R3::vulkan {

class RenderPass {
public:
    R3_CTOR_DEFAULT(RenderPass);
    R3_COPY_DELETE(RenderPass);
    R3_MOVE_DEFAULT(RenderPass);

private:
    RenderPass(RenderContext& ctx, const VkRenderPassCreateInfo& renderPassInfo);

public:
    ~RenderPass() noexcept;

    VkRenderPass renderPass() const noexcept { return m_renderPass; }

private:
    Handle<VkDevice> m_device;
    Handle<VkRenderPass> m_renderPass;

private:
    friend class RenderPassBuilder;
};

class RenderPassBuilder {
public:
    /// Add MSAA color attachment (will be used as color attachment in subpass)
    RenderPassBuilder& addMSAAColorAttachment(VkFormat format, VkSampleCountFlagBits samples);

    /// Add regular color attachment (no MSAA)
    RenderPassBuilder& addColorAttachment(VkFormat format);

    /// Set depth attachment
    RenderPassBuilder& setDepthStencilAttachment(VkFormat format,
                                                 VkSampleCountFlagBits samples = VK_SAMPLE_COUNT_1_BIT,
                                                 bool storeDepth               = false);

    RenderPass build(RenderContext& ctx);

private:
    std::vector<VkAttachmentDescription> m_attachments;
    std::vector<VkAttachmentReference> m_colorAttachmentRefs;
    std::vector<VkAttachmentReference> m_resolveAttachmentRefs;
    VkAttachmentReference m_depthStencilAttachmentRef{VK_ATTACHMENT_UNUSED, VK_IMAGE_LAYOUT_UNDEFINED};
    bool m_hasDepthStencilAttachment = false;
};

} // namespace R3::vulkan
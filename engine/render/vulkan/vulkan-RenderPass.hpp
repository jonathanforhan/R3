#pragma once

#include <span>
#include <vector>
#include <vulkan/vulkan_core.h>
#include "Class.hpp"
#include "Types.hpp"
#include "vulkan-Handle.hpp"
#include "vulkan-RenderContext.hpp"

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
    RenderPassBuilder& addMSAAColorAttachment(VkFormat format, VkSampleCountFlagBits samples);

    RenderPassBuilder& addSwapchainColorAttachment(VkFormat format);

    RenderPassBuilder& addOffscreenColorAttachment(VkFormat format);

    RenderPassBuilder& setDepthStencilAttachment(VkFormat format, bool storeDepth = false);

    RenderPassBuilder& setResolveTarget(usize msaaIndex, usize resolveTargetIndex);

    RenderPass build(RenderContext& ctx);

private:
    std::vector<VkAttachmentDescription> m_attachments;
    std::vector<VkAttachmentReference> m_colorAttachmentRefs;
    std::vector<VkAttachmentReference> m_resolveAttachmentRefs;
    VkAttachmentReference m_depthStencilAttachmentRef{VK_ATTACHMENT_UNUSED, VK_IMAGE_LAYOUT_UNDEFINED};
    bool m_hasDepthStencilAttachment = false;
};

} // namespace R3::vulkan
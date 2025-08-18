#if R3_VULKAN

#include "vulkan-RenderPass.hpp"

#include <cstdint>
#include <span>
#include <vector>
#include <vulkan/vulkan_core.h>
#include "vulkan-Check.hpp"
#include "vulkan-RenderContext.hpp"

namespace R3::vulkan {

void RenderPass::create(RenderContext& ctx, std::span<const VkAttachmentDescription> attachments) {
    m_device = ctx.device();

    auto colorAttachmentRefs = std::vector<VkAttachmentReference>(attachments.size());

    for (size_t i = 0; i < attachments.size(); ++i) {
        // For now, assume all attachments are color attachments
        const VkAttachmentReference colorAttachmentRef = {
            .attachment = static_cast<uint32_t>(i),
            .layout     = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        };
        colorAttachmentRefs[i] = colorAttachmentRef;
    }

    const VkSubpassDescription subpass = {
        .flags                   = 0,
        .pipelineBindPoint       = VK_PIPELINE_BIND_POINT_GRAPHICS,
        .inputAttachmentCount    = 0,
        .pInputAttachments       = nullptr,
        .colorAttachmentCount    = static_cast<uint32_t>(colorAttachmentRefs.size()),
        .pColorAttachments       = colorAttachmentRefs.data(),
        .pResolveAttachments     = nullptr,
        .pDepthStencilAttachment = nullptr,
        .preserveAttachmentCount = 0,
        .pPreserveAttachments    = nullptr,
    };

    const VkSubpassDependency dependency = {
        .srcSubpass      = VK_SUBPASS_EXTERNAL,
        .dstSubpass      = 0,
        .srcStageMask    = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
        .dstStageMask    = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
        .srcAccessMask   = 0,
        .dstAccessMask   = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
        .dependencyFlags = 0,
    };

    const VkRenderPassCreateInfo renderPassInfo = {
        .sType           = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
        .pNext           = nullptr,
        .flags           = 0,
        .attachmentCount = static_cast<uint32_t>(attachments.size()),
        .pAttachments    = attachments.data(),
        .subpassCount    = 1,
        .pSubpasses      = &subpass,
        .dependencyCount = 1,
        .pDependencies   = &dependency,
    };

    VK_CHECK(vkCreateRenderPass(m_device, &renderPassInfo, nullptr, &m_renderPass));
}

void RenderPass::destroy() noexcept {
    if (m_device != VK_NULL_HANDLE) {
        if (m_renderPass != VK_NULL_HANDLE) {
            vkDestroyRenderPass(m_device, m_renderPass, nullptr);
            m_renderPass = VK_NULL_HANDLE;
        }
    }
    m_device = VK_NULL_HANDLE;
}

} // namespace R3::vulkan

#endif // R3_VULKAN
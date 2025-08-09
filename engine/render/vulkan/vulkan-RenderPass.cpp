#include "vulkan-RenderPass.hpp"

#include <cstdint>
#include <span>
#include <vector>
#include <vulkan/vulkan_core.h>
#include "vulkan-Check.hpp"
#include "vulkan-RenderContext.hpp"

namespace R3 {

void RenderPass::create(RenderContext& ctx, std::span<AttachmentDescription> attachments) noexcept(false) {
    m_device = ctx.device();

    auto attachmentsDescs    = std::vector<VkAttachmentDescription>(attachments.size());
    auto colorAttachmentRefs = std::vector<VkAttachmentReference>(attachments.size());

    for (size_t i = 0; i < attachments.size(); ++i) {
        const auto& desc = attachments[i];

        VkAttachmentDescription attachmentDesc = {
            .flags          = 0,
            .format         = desc.format,
            .samples        = desc.samples,
            .loadOp         = desc.loadOp,
            .storeOp        = desc.storeOp,
            .stencilLoadOp  = desc.stencilLoadOp,
            .stencilStoreOp = desc.stencilStoreOp,
            .initialLayout  = desc.initialLayout,
            .finalLayout    = desc.finalLayout,
        };
        attachmentsDescs[i] = attachmentDesc;

        // For now, assume all attachments are color attachments
        VkAttachmentReference colorAttachmentRef = {
            .attachment = static_cast<uint32_t>(i),
            .layout     = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        };
        colorAttachmentRefs[i] = colorAttachmentRef;
    }

    VkSubpassDescription subpass = {
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

    VkSubpassDependency dependency = {
        .srcSubpass      = VK_SUBPASS_EXTERNAL,
        .dstSubpass      = 0,
        .srcStageMask    = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
        .dstStageMask    = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
        .srcAccessMask   = 0,
        .dstAccessMask   = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
        .dependencyFlags = 0,
    };

    VkRenderPassCreateInfo renderPassInfo = {
        .sType           = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
        .pNext           = nullptr,
        .flags           = 0,
        .attachmentCount = static_cast<uint32_t>(attachmentsDescs.size()),
        .pAttachments    = attachmentsDescs.data(),
        .subpassCount    = 1,
        .pSubpasses      = &subpass,
        .dependencyCount = 1,
        .pDependencies   = &dependency,
    };

    VK_CHECK(vkCreateRenderPass(m_device, &renderPassInfo, nullptr, &m_renderPass));
}

void RenderPass::destroy() noexcept(true) {
    if (m_renderPass != VK_NULL_HANDLE && m_device != VK_NULL_HANDLE) {
        vkDestroyRenderPass(m_device, m_renderPass, nullptr);
        m_renderPass = VK_NULL_HANDLE;
    }
    m_device = VK_NULL_HANDLE;
}

} // namespace R3
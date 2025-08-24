#include "vulkan-RenderPass.hpp"

#include <cassert>
#include <vector>
#include <vulkan/vulkan_core.h>
#include "api/Types.hpp"
#include "vulkan-Check.hpp"
#include "vulkan-Handle.hpp"
#include "vulkan-RenderContext.hpp"

namespace R3::vulkan {

RenderPass::RenderPass(RenderContext& ctx, const VkRenderPassCreateInfo& renderPassInfo) {
    m_device = ctx.device();
    VK_CHECK(vkCreateRenderPass(m_device, &renderPassInfo, nullptr, &*m_renderPass));
}

RenderPass::~RenderPass() noexcept {
    if (m_device) {
        vkDestroyRenderPass(m_device, m_renderPass, nullptr);
    }
}

RenderPassBuilder& RenderPassBuilder::addMSAAColorAttachment(VkFormat format, VkSampleCountFlagBits samples) {
    assert(samples != VK_SAMPLE_COUNT_1_BIT && "MSAA color attachment must have samples > 1");

    m_attachments.push_back({
        .flags          = 0,
        .format         = format,
        .samples        = samples,
        .loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR,
        .storeOp        = VK_ATTACHMENT_STORE_OP_DONT_CARE,
        .stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
        .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
        .initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED,
        .finalLayout    = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
    });

    m_colorAttachmentRefs.push_back({
        .attachment = static_cast<uint32>(m_attachments.size() - 1),
        .layout     = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
    });

    m_attachments.push_back({
        .flags          = 0,
        .format         = format,
        .samples        = VK_SAMPLE_COUNT_1_BIT,           // Resolve targets are always 1x
        .loadOp         = VK_ATTACHMENT_LOAD_OP_DONT_CARE, // Will be written by resolve
        .storeOp        = VK_ATTACHMENT_STORE_OP_STORE,    // Store the resolved result
        .stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
        .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
        .initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED,
        .finalLayout    = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, // Assume going to swapchain
    });

    m_resolveAttachmentRefs.push_back({
        .attachment = static_cast<uint32>(m_attachments.size() - 1),
        .layout     = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
    });

    return *this;
}

RenderPassBuilder& RenderPassBuilder::addColorAttachment(VkFormat format, VkSampleCountFlagBits samples) {
    m_attachments.push_back({
        .flags          = 0,
        .format         = format,
        .samples        = VK_SAMPLE_COUNT_1_BIT,
        .loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR,
        .storeOp        = VK_ATTACHMENT_STORE_OP_STORE,
        .stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
        .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
        .initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED,
        .finalLayout    = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
    });

    m_colorAttachmentRefs.push_back({
        .attachment = static_cast<uint32>(m_attachments.size() - 1),
        .layout     = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
    });

    // Offscreen attachments don't typically need resolve targets
    m_resolveAttachmentRefs.push_back({
        .attachment = VK_ATTACHMENT_UNUSED,
        .layout     = VK_IMAGE_LAYOUT_UNDEFINED,
    });

    return *this;
}

RenderPassBuilder& RenderPassBuilder::setDepthStencilAttachment(VkFormat format,
                                                                VkSampleCountFlagBits samples,
                                                                bool storeDepth) {
    assert(m_hasDepthStencilAttachment == false && "Depth/stencil attachment already set");

    m_attachments.push_back({
        .flags          = 0,
        .format         = format,
        .samples        = samples,
        .loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR,
        .storeOp        = storeDepth ? VK_ATTACHMENT_STORE_OP_STORE : VK_ATTACHMENT_STORE_OP_DONT_CARE,
        .stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
        .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
        .initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED,
        .finalLayout    = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
    });

    m_depthStencilAttachmentRef = {
        .attachment = static_cast<uint32>(m_attachments.size() - 1),
        .layout     = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
    };

    m_hasDepthStencilAttachment = true;
    return *this;
}

RenderPass RenderPassBuilder::build(RenderContext& ctx) {
    // Create the subpass description
    const VkSubpassDescription subpass = {
        .flags                   = 0,
        .pipelineBindPoint       = VK_PIPELINE_BIND_POINT_GRAPHICS,
        .inputAttachmentCount    = 0,
        .pInputAttachments       = nullptr,
        .colorAttachmentCount    = static_cast<uint32>(m_colorAttachmentRefs.size()),
        .pColorAttachments       = m_colorAttachmentRefs.data(),
        .pResolveAttachments     = m_resolveAttachmentRefs.empty() ? nullptr : m_resolveAttachmentRefs.data(),
        .pDepthStencilAttachment = m_hasDepthStencilAttachment ? &m_depthStencilAttachmentRef : nullptr,
        .preserveAttachmentCount = 0,
        .pPreserveAttachments    = nullptr,
    };

    // Standard subpass dependency for color + depth
    const VkSubpassDependency dependency = {
        .srcSubpass      = VK_SUBPASS_EXTERNAL,
        .dstSubpass      = 0,
        .srcStageMask    = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT,
        .dstStageMask    = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
        .srcAccessMask   = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
        .dstAccessMask   = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
        .dependencyFlags = 0,
    };

    const VkRenderPassCreateInfo renderPassInfo = {
        .sType           = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
        .pNext           = nullptr,
        .flags           = {},
        .attachmentCount = static_cast<uint32>(m_attachments.size()),
        .pAttachments    = m_attachments.data(),
        .subpassCount    = 1,
        .pSubpasses      = &subpass,
        .dependencyCount = 1,
        .pDependencies   = &dependency,
    };

    return RenderPass(ctx, renderPassInfo);
}

} // namespace R3::vulkan

#if R3_VULKAN

#include "vulkan-DepthBuffer.hxx"
#include "vulkan-LogicalDevice.hxx"
#include "vulkan-PhysicalDevice.hxx"
#include "vulkan-RenderPass.hxx"
#include <api/Assert.hpp>
#include <api/Types.hpp>
#include <vector>

namespace R3::vulkan {

RenderPass::RenderPass(const RenderPassSpecification& spec)
    : m_device(spec.device.vk()) {
    const VkAttachmentDescription colorAttachment = {
        .flags          = {},
        .format         = spec.colorAttachment.format,
        .samples        = spec.colorAttachment.sampleCount,
        .loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR,
        .storeOp        = VK_ATTACHMENT_STORE_OP_STORE,
        .stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
        .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
        .initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED,
        .finalLayout    = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
    };
    const VkAttachmentReference colorAttachmentReference = {
        .attachment = 0,
        .layout     = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
    };

    const VkFormat depthFormat = DepthBuffer::querySupportedDepthFormat(
        spec.physicalDevice.vk(), VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);

    const VkAttachmentDescription depthAttachment = {
        .flags          = {},
        .format         = depthFormat,
        .samples        = spec.depthAttachment.sampleCount,
        .loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR,
        .storeOp        = VK_ATTACHMENT_STORE_OP_DONT_CARE,
        .stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
        .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
        .initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED,
        .finalLayout    = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
    };
    const VkAttachmentReference depthAttachmentReference = {
        .attachment = 1,
        .layout     = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
    };

    // color resolve attachment, to convert color attach from MSAA to normal image
    const VkAttachmentDescription colorAttachmentResolve = {
        .flags          = {},
        .format         = spec.colorAttachment.format,
        .samples        = VK_SAMPLE_COUNT_1_BIT,
        .loadOp         = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
        .storeOp        = VK_ATTACHMENT_STORE_OP_STORE,
        .stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
        .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
        .initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED,
        .finalLayout    = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
    };
    const VkAttachmentReference colorAttachmentResolveReference = {
        .attachment = 2,
        .layout     = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
    };

    std::vector<VkAttachmentDescription> attachments;

    const bool msaa = spec.colorAttachment.sampleCount > 1; // Color Attachment Resolve, only present if MSAA is enabled

    if (msaa) {
        attachments = {colorAttachment, depthAttachment, colorAttachmentResolve};
    } else {
        attachments = {colorAttachment, depthAttachment};
    }

    const VkSubpassDescription subpassDescription = {
        .flags                   = {},
        .pipelineBindPoint       = VK_PIPELINE_BIND_POINT_GRAPHICS,
        .inputAttachmentCount    = 0,
        .pInputAttachments       = nullptr,
        .colorAttachmentCount    = 1,
        .pColorAttachments       = &colorAttachmentReference,
        .pResolveAttachments     = msaa ? &colorAttachmentResolveReference : nullptr,
        .pDepthStencilAttachment = &depthAttachmentReference,
        .preserveAttachmentCount = 0,
        .pPreserveAttachments    = nullptr,
    };

    const VkSubpassDependency subpassDependency = {
        .srcSubpass      = VK_SUBPASS_EXTERNAL,
        .dstSubpass      = 0,
        .srcStageMask    = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
        .dstStageMask    = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
        .srcAccessMask   = {},
        .dstAccessMask   = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
        .dependencyFlags = {},
    };

    const VkRenderPassCreateInfo renderPassCreateInfo = {
        .sType           = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
        .pNext           = nullptr,
        .flags           = {},
        .attachmentCount = static_cast<uint32>(attachments.size()),
        .pAttachments    = attachments.data(),
        .subpassCount    = 1,
        .pSubpasses      = &subpassDescription,
        .dependencyCount = 1,
        .pDependencies   = &subpassDependency,
    };
    VkResult result = vkCreateRenderPass(spec.device.vk(), &renderPassCreateInfo, nullptr, &m_handle);
    ENSURE(result == VK_SUCCESS);
}

RenderPass::~RenderPass() {
    vkDestroyRenderPass(m_device, m_handle, nullptr);
}

} // namespace R3::vulkan

#endif // R3_VULKAN

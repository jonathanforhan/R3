#pragma once

#include <vector>
#include <vulkan/vulkan_core.h>

namespace R3 {

class RenderContext;

struct AttachmentDescription {
    VkFormat format;
    VkSampleCountFlagBits samples      = VK_SAMPLE_COUNT_1_BIT;
    VkAttachmentLoadOp loadOp          = VK_ATTACHMENT_LOAD_OP_CLEAR;
    VkAttachmentStoreOp storeOp        = VK_ATTACHMENT_STORE_OP_STORE;
    VkAttachmentLoadOp stencilLoadOp   = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    VkAttachmentStoreOp stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    VkImageLayout initialLayout        = VK_IMAGE_LAYOUT_UNDEFINED;
    VkImageLayout finalLayout          = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
};

class RenderPass {
public:
    void create(RenderContext& ctx, const std::vector<AttachmentDescription>& attachments) noexcept(false);

    void destroy() noexcept(true);

    VkRenderPass handle() const { return m_renderPass; }

    bool isValid() const { return m_renderPass != VK_NULL_HANDLE; }

private:
    VkDevice m_device         = VK_NULL_HANDLE;
    VkRenderPass m_renderPass = VK_NULL_HANDLE;
};

} // namespace R3
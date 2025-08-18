#if R3_VULKAN

#include "vulkan-Framebuffer.hpp"

#include <span>
#include <vulkan/vulkan_core.h>
#include "Types.hpp"
#include "vulkan-Check.hpp"
#include "vulkan-RenderContext.hpp"
#include "vulkan-RenderPass.hpp"

namespace R3::vulkan {

void Framebuffer::create(RenderContext& ctx,
                         RenderPass& renderPass,
                         std::span<const VkImageView> attachments,
                         VkExtent2D extent) {
    m_device = ctx.device();

    const VkFramebufferCreateInfo framebufferInfo = {
        .sType           = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
        .pNext           = nullptr,
        .flags           = 0,
        .renderPass      = renderPass.renderPass(),
        .attachmentCount = static_cast<uint32>(attachments.size()),
        .pAttachments    = attachments.data(),
        .width           = extent.width,
        .height          = extent.height,
        .layers          = 1,
    };
    VK_CHECK(vkCreateFramebuffer(m_device, &framebufferInfo, nullptr, &m_framebuffer));
}

void Framebuffer::destroy() noexcept {
    if (m_device != VK_NULL_HANDLE) {
        vkDestroyFramebuffer(m_device, m_framebuffer, nullptr);
    }
    m_device = VK_NULL_HANDLE;
}

} // namespace R3::vulkan

#endif // R3_VULKAN
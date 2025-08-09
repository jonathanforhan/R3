#if R3_VULKAN

#include "render/Framebuffer.hpp"

#include <span>
#include <vulkan/vulkan_core.h>
#include "Types.hpp"
#include "render/RenderContext.hpp"
#include "render/RenderPass.hpp"
#include "vulkan-Check.hpp"

namespace R3 {

void Framebuffer::create(RenderContext& ctx,
                         RenderPass& renderPass,
                         std::span<const VkImageView> attachments,
                         uvec2 extent) {
    m_device = ctx.device();

    const VkFramebufferCreateInfo framebufferInfo = {
        .sType           = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
        .pNext           = nullptr,
        .flags           = 0,
        .renderPass      = renderPass.handle(),
        .attachmentCount = static_cast<uint32>(attachments.size()),
        .pAttachments    = attachments.data(),
        .width           = extent.x,
        .height          = extent.y,
        .layers          = 1,
    };
    VK_CHECK(vkCreateFramebuffer(m_device, &framebufferInfo, nullptr, &m_framebuffer));
}

void Framebuffer::destroy() noexcept {
    vkDestroyFramebuffer(m_device, m_framebuffer, nullptr);
    m_device = VK_NULL_HANDLE;
}

} // namespace R3

#endif // R3_VULKAN
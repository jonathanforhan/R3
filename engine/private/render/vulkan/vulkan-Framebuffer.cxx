#if R3_VULKAN

#include "vulkan-Framebuffer.hxx"

#include "vulkan-LogicalDevice.hxx"
#include "vulkan-RenderPass.hxx"
#include <api/Assert.hpp>
#include <api/Types.hpp>
#include <vulkan/vulkan.h>

namespace R3::vulkan {
Framebuffer::Framebuffer(const FramebufferSpecification& spec)
    : m_device(spec.device) {
    const VkFramebufferCreateInfo framebufferCreateInfo = {
        .sType           = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
        .pNext           = nullptr,
        .flags           = {},
        .renderPass      = spec.renderPass,
        .attachmentCount = static_cast<uint32>(spec.attachments.size()),
        .pAttachments    = spec.attachments.data(),
        .width           = spec.extent.width,
        .height          = spec.extent.height,
        .layers          = 1,
    };

    VkResult result = vkCreateFramebuffer(m_device, &framebufferCreateInfo, nullptr, &m_handle);
    ENSURE(result == VK_SUCCESS);
}

Framebuffer::~Framebuffer() {
    vkDestroyFramebuffer(m_device, m_handle, nullptr);
}

} // namespace R3::vulkan

#endif // R3_VULKAN

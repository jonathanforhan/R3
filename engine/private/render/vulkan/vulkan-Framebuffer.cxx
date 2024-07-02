#if R3_VULKAN

#include "vulkan-Framebuffer.hxx"

#include "vulkan-LogicalDevice.hxx"
#include "vulkan-RenderPass.hxx"
#include <api/Log.hpp>
#include <api/Types.hpp>
#include <expected>
#include <vulkan/vulkan.h>

namespace R3::vulkan {

Result<Framebuffer> Framebuffer::create(const FramebufferSpecification& spec) {
    Framebuffer self;
    self.m_device = spec.device.vk();

    const VkFramebufferCreateInfo framebufferCreateInfo = {
        .sType           = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
        .pNext           = nullptr,
        .flags           = {},
        .renderPass      = spec.renderPass.vk(),
        .attachmentCount = static_cast<uint32>(spec.attachments.size()),
        .pAttachments    = spec.attachments.data(),
        .width           = spec.extent.width,
        .height          = spec.extent.height,
        .layers          = 1,
    };

    if (VkResult result = vkCreateFramebuffer(self.m_device, &framebufferCreateInfo, nullptr, &self.m_handle)) {
        R3_LOG(Error, "vkCreateFramebuffer failure {}", (int)result);
        return std::unexpected(Error::InitializationFailure);
    }

    return self;
}

Framebuffer::~Framebuffer() {
    vkDestroyFramebuffer(m_device, m_handle, nullptr);
}

} // namespace R3::vulkan

#endif // R3_VULKAN

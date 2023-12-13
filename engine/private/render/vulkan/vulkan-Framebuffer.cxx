#if R3_VULKAN

#include "render/Framebuffer.hpp"

#include <vulkan/vulkan.h>
#include "api/Check.hpp"
#include "api/Ensure.hpp"
#include "render/ImageView.hpp"
#include "render/Swapchain.hpp"
#include "render/RenderPass.hpp"
#include "render/LogicalDevice.hpp"

namespace R3 {

Framebuffer::Framebuffer(const FramebufferSpecification& spec)
    : m_spec(spec) {

    const VkImageView imageView = m_spec.imageView->as<VkImageView>();
    const VkImageView attachments[] = {
        imageView,
    };

    const VkFramebufferCreateInfo framebufferCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
        .pNext = nullptr,
        .flags = {},
        .renderPass = m_spec.renderPass->as<VkRenderPass>(),
        .attachmentCount = 1,
        .pAttachments = attachments,
        .width = m_spec.swapchain->extent().x,
        .height = m_spec.swapchain->extent().y,
        .layers = 1,
    };

    VkFramebuffer tmp;
    VkResult result = vkCreateFramebuffer(m_spec.logicalDevice->as<VkDevice>(), &framebufferCreateInfo, nullptr, &tmp);
    ENSURE(result == VK_SUCCESS);
    setHandle(tmp);
}

Framebuffer::~Framebuffer() {
    if (validHandle()) {
        vkDestroyFramebuffer(m_spec.logicalDevice->as<VkDevice>(), as<VkFramebuffer>(), nullptr);
    }
}

} // namespace R3

#endif // R3_VULKAN
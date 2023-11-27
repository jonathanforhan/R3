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

void Framebuffer::create(const FramebufferSpecification& spec) {
    CHECK(spec.logicalDevice != nullptr);
    CHECK(spec.swapchain != nullptr);
    CHECK(spec.imageView != nullptr);
    CHECK(spec.renderPass != nullptr);
    m_spec = spec;

    VkImageView imageView = m_spec.imageView->handle<VkImageView>();
    VkImageView attachments[] = {
        imageView,
    };

    VkFramebufferCreateInfo framebufferCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0U,
        .renderPass = m_spec.renderPass->handle<VkRenderPass>(),
        .attachmentCount = 1,
        .pAttachments = attachments,
        .width = m_spec.swapchain->extent().x,
        .height = m_spec.swapchain->extent().y,
        .layers = 1,
    };

    ENSURE(vkCreateFramebuffer(m_spec.logicalDevice->handle<VkDevice>(),
                               &framebufferCreateInfo,
                               nullptr,
                               handlePtr<VkFramebuffer*>()) == VK_SUCCESS);
}

void Framebuffer::destroy() {
    vkDestroyFramebuffer(m_spec.logicalDevice->handle<VkDevice>(), handle<VkFramebuffer>(), nullptr);
}

} // namespace R3

#endif // R3_VULKAN
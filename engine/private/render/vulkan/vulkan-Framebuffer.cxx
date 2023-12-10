#if R3_VULKAN

#include "render/Framebuffer.hpp"

#include <vulkan/vulkan.hpp>
#include "api/Check.hpp"
#include "render/ImageView.hpp"
#include "render/Swapchain.hpp"
#include "render/RenderPass.hpp"
#include "render/LogicalDevice.hpp"

namespace R3 {

Framebuffer::Framebuffer(const FramebufferSpecification& spec)
    : m_spec(spec) {

    vk::ImageView imageView = m_spec.imageView->as<vk::ImageView>();
    vk::ImageView attachments[] = {
        imageView,
    };

    vk::FramebufferCreateInfo framebufferCreateInfo = {
        .sType = vk::StructureType::eFramebufferCreateInfo,
        .pNext = nullptr,
        .flags = {},
        .renderPass = m_spec.renderPass->as<vk::RenderPass>(),
        .attachmentCount = 1,
        .pAttachments = attachments,
        .width = m_spec.swapchain->extent().x,
        .height = m_spec.swapchain->extent().y,
        .layers = 1,
    };

    setHandle(m_spec.logicalDevice->as<vk::Device>().createFramebuffer(framebufferCreateInfo));
}

Framebuffer::~Framebuffer() {
    if (validHandle()) {
        m_spec.logicalDevice->as<vk::Device>().destroyFramebuffer(as<vk::Framebuffer>());
    }
}

} // namespace R3

#endif // R3_VULKAN
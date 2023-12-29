#if R3_VULKAN

#include "render/Framebuffer.hxx"

#include <vulkan/vulkan.hpp>
#include "render/ImageView.hxx"
#include "render/LogicalDevice.hxx"
#include "render/RenderPass.hxx"
#include "render/Swapchain.hxx"

namespace R3 {

Framebuffer::Framebuffer(const FramebufferSpecification& spec)
    : m_logicalDevice(&spec.logicalDevice) {
    // NOTE this format [ COLOR, DEPTH, SWAPCHAIN_IMAGE ]
    // IS important. It is mirrored in the layout of the renderpass attachments
    const vk::ImageView attachments[] = {
        spec.colorBufferImageView.as<vk::ImageView>(),
        spec.depthBufferImageView.as<vk::ImageView>(),
        spec.swapchainImageView.as<vk::ImageView>(),
    };

    const vk::FramebufferCreateInfo framebufferCreateInfo = {
        .sType = vk::StructureType::eFramebufferCreateInfo,
        .pNext = nullptr,
        .flags = {},
        .renderPass = spec.renderPass.as<vk::RenderPass>(),
        .attachmentCount = static_cast<uint32>(std::size(attachments)),
        .pAttachments = attachments,
        .width = spec.swapchain.extent().x,
        .height = spec.swapchain.extent().y,
        .layers = 1,
    };

    setHandle(m_logicalDevice->as<vk::Device>().createFramebuffer(framebufferCreateInfo));
}

Framebuffer::~Framebuffer() {
    if (validHandle()) {
        m_logicalDevice->as<vk::Device>().destroyFramebuffer(as<vk::Framebuffer>());
    }
}

} // namespace R3

#endif // R3_VULKAN
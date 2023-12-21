#if R3_VULKAN

#include "render/Framebuffer.hpp"

#include <array>
#include <vulkan/vulkan.hpp>
#include "api/Check.hpp"
#include "render/ImageView.hpp"
#include "render/LogicalDevice.hpp"
#include "render/RenderPass.hpp"
#include "render/Swapchain.hpp"

namespace R3 {

Framebuffer::Framebuffer(const FramebufferSpecification& spec)
    : m_logicalDevice(spec.logicalDevice) {
    const vk::ImageView swapchainImageView = spec.swapchainImageView->as<vk::ImageView>();
    const vk::ImageView depthBufferImageView = spec.depthBufferImageView->as<vk::ImageView>();
    const std::array<vk::ImageView, 2> attachments = {
        swapchainImageView,
        depthBufferImageView,
    };

    const vk::FramebufferCreateInfo framebufferCreateInfo = {
        .sType = vk::StructureType::eFramebufferCreateInfo,
        .pNext = nullptr,
        .flags = {},
        .renderPass = spec.renderPass->as<vk::RenderPass>(),
        .attachmentCount = static_cast<uint32>(attachments.size()),
        .pAttachments = attachments.data(),
        .width = spec.swapchain->extent().x,
        .height = spec.swapchain->extent().y,
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
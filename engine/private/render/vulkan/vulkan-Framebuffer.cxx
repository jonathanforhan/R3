#if R3_VULKAN

#include "render/Framebuffer.hpp"

#include <vulkan/vulkan.hpp>
#include "render/ImageView.hpp"
#include "render/LogicalDevice.hpp"
#include "render/RenderPass.hpp"
#include "render/Swapchain.hpp"

namespace R3 {

Framebuffer::Framebuffer(const FramebufferSpecification& spec)
    : m_logicalDevice(&spec.logicalDevice),
      m_extent(spec.extent) {
    std::vector<vk::ImageView> attachments(spec.attachments.size());
    std::ranges::transform(spec.attachments, attachments.begin(), [](const auto& x) { return x->as<vk::ImageView>(); });

    const vk::FramebufferCreateInfo framebufferCreateInfo = {
        .sType = vk::StructureType::eFramebufferCreateInfo,
        .pNext = nullptr,
        .flags = {},
        .renderPass = spec.renderPass.as<vk::RenderPass>(),
        .attachmentCount = static_cast<uint32>(attachments.size()),
        .pAttachments = attachments.data(),
        .width = spec.extent.x,
        .height = spec.extent.y,
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
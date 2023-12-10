#if R3_VULKAN

#include "render/RenderPass.hpp"

#include <vulkan/vulkan.hpp>
#include "api/Check.hpp"
#include "api/Ensure.hpp"
#include "render/LogicalDevice.hpp"
#include "render/Swapchain.hpp"

namespace R3 {

RenderPass::RenderPass(const RenderPassSpecification& spec)
    : m_spec(spec) {

    vk::AttachmentDescription colorAttachment = {
        .flags = {},
        .format = (vk::Format)m_spec.swapchain->surfaceFormat(),
        .samples = vk::SampleCountFlagBits::e1,
        .loadOp = vk::AttachmentLoadOp::eClear,
        .storeOp = vk::AttachmentStoreOp::eStore,
        .stencilLoadOp = vk::AttachmentLoadOp::eDontCare,
        .stencilStoreOp = vk::AttachmentStoreOp::eDontCare,
        .initialLayout = vk::ImageLayout::eUndefined,
        .finalLayout = vk::ImageLayout::ePresentSrcKHR,
    };

    vk::AttachmentReference colorAttachmentReference = {
        .attachment = 0,
        .layout = vk::ImageLayout::eColorAttachmentOptimal,
    };

    vk::SubpassDescription subpassDescription = {
        .flags = {},
        .pipelineBindPoint = vk::PipelineBindPoint::eGraphics,
        .inputAttachmentCount = 0,
        .pInputAttachments = nullptr,
        .colorAttachmentCount = 1,
        .pColorAttachments = &colorAttachmentReference,
        .pResolveAttachments = nullptr,
        .pDepthStencilAttachment = nullptr,
        .preserveAttachmentCount = 0,
        .pPreserveAttachments = nullptr,
    };

    vk::SubpassDependency subpassDependency = {
        .srcSubpass = VK_SUBPASS_EXTERNAL,
        .dstSubpass = 0,
        .srcStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput,
        .dstStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput,
        .srcAccessMask = {},
        .dstAccessMask = vk::AccessFlagBits::eColorAttachmentWrite,
        .dependencyFlags = {},
    };

    vk::RenderPassCreateInfo renderPassCreateInfo = {
        .sType = vk::StructureType::eRenderPassCreateInfo,
        .pNext = nullptr,
        .flags = {},
        .attachmentCount = 1,
        .pAttachments = &colorAttachment,
        .subpassCount = 1,
        .pSubpasses = &subpassDescription,
        .dependencyCount = 1,
        .pDependencies = &subpassDependency,
    };

    setHandle(m_spec.logicalDevice->as<vk::Device>().createRenderPass(renderPassCreateInfo));
}

RenderPass::~RenderPass() {
    if (validHandle()) {
        m_spec.logicalDevice->as<vk::Device>().destroyRenderPass(as<vk::RenderPass>());
    }
}

} // namespace R3

#endif // R3_VULKAN
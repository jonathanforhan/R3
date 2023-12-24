#if R3_VULKAN

#include "render/RenderPass.hpp"

#include <vulkan/vulkan.hpp>
#include "render/LogicalDevice.hpp"
#include "render/PhysicalDevice.hpp"
#include "render/Swapchain.hpp"
#include "vulkan-DepthBufferFormat.hxx"

namespace R3 {

RenderPass::RenderPass(const RenderPassSpecification& spec)
    : m_logicalDevice(&spec.logicalDevice) {
    const vk::AttachmentDescription colorAttachment = {
        .flags = {},
        .format = vk::Format(spec.swapchain.surfaceFormat()),
        .samples = vk::SampleCountFlagBits(spec.physicalDevice.sampleCount()),
        .loadOp = vk::AttachmentLoadOp::eClear,
        .storeOp = vk::AttachmentStoreOp::eStore,
        .stencilLoadOp = vk::AttachmentLoadOp::eDontCare,
        .stencilStoreOp = vk::AttachmentStoreOp::eDontCare,
        .initialLayout = vk::ImageLayout::eUndefined,
        .finalLayout = vk::ImageLayout::eColorAttachmentOptimal,
    };
    const vk::AttachmentReference colorAttachmentReference = {
        .attachment = 0,
        .layout = vk::ImageLayout::eColorAttachmentOptimal,
    };

    const vk::AttachmentDescription depthAttachment = {
        .flags = {},
        .format = vulkan::getSupportedDepthFormat(spec.physicalDevice.as<vk::PhysicalDevice>(),
                                                  vk::ImageTiling::eOptimal,
                                                  vk::FormatFeatureFlagBits::eDepthStencilAttachment),
        .samples = vk::SampleCountFlagBits(spec.physicalDevice.sampleCount()),
        .loadOp = vk::AttachmentLoadOp::eClear,
        .storeOp = vk::AttachmentStoreOp::eDontCare,
        .stencilLoadOp = vk::AttachmentLoadOp::eDontCare,
        .stencilStoreOp = vk::AttachmentStoreOp::eDontCare,
        .initialLayout = vk::ImageLayout::eUndefined,
        .finalLayout = vk::ImageLayout::eDepthStencilAttachmentOptimal,
    };
    const vk::AttachmentReference depthAttachmentReference = {
        .attachment = 1,
        .layout = vk::ImageLayout::eDepthStencilAttachmentOptimal,
    };

    // color resolve attachment, to convert color attach from MSAA to normal image
    const vk::AttachmentDescription colorAttachmentResolve = {
        .flags = {},
        .format = vk::Format(spec.swapchain.surfaceFormat()),
        .samples = vk::SampleCountFlagBits::e1,
        .loadOp = vk::AttachmentLoadOp::eDontCare,
        .storeOp = vk::AttachmentStoreOp::eStore,
        .stencilLoadOp = vk::AttachmentLoadOp::eDontCare,
        .stencilStoreOp = vk::AttachmentStoreOp::eDontCare,
        .initialLayout = vk::ImageLayout::eUndefined,
        .finalLayout = vk::ImageLayout::ePresentSrcKHR,
    };
    const vk::AttachmentReference colorAttachmentResolveReference = {
        .attachment = 2,
        .layout = vk::ImageLayout::eColorAttachmentOptimal,
    };

    const vk::SubpassDescription subpassDescription = {
        .flags = {},
        .pipelineBindPoint = vk::PipelineBindPoint::eGraphics,
        .inputAttachmentCount = 0,
        .pInputAttachments = nullptr,
        .colorAttachmentCount = 1,
        .pColorAttachments = &colorAttachmentReference,
        .pResolveAttachments = &colorAttachmentResolveReference,
        .pDepthStencilAttachment = &depthAttachmentReference,
        .preserveAttachmentCount = 0,
        .pPreserveAttachments = nullptr,
    };

    const vk::SubpassDependency subpassDependency = {
        .srcSubpass = vk::SubpassExternal,
        .dstSubpass = 0,
        .srcStageMask =
            vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests,
        .dstStageMask =
            vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests,
        .srcAccessMask = {},
        .dstAccessMask = vk::AccessFlagBits::eColorAttachmentWrite | vk::AccessFlagBits::eDepthStencilAttachmentWrite,
        .dependencyFlags = {},
    };

    const vk::AttachmentDescription attachments[] = {colorAttachment, depthAttachment, colorAttachmentResolve};

    const vk::RenderPassCreateInfo renderPassCreateInfo = {
        .sType = vk::StructureType::eRenderPassCreateInfo,
        .pNext = nullptr,
        .flags = {},
        .attachmentCount = static_cast<uint32>(std::size(attachments)),
        .pAttachments = attachments,
        .subpassCount = 1,
        .pSubpasses = &subpassDescription,
        .dependencyCount = 1,
        .pDependencies = &subpassDependency,
    };

    setHandle(m_logicalDevice->as<vk::Device>().createRenderPass(renderPassCreateInfo));
}

RenderPass::~RenderPass() {
    if (validHandle()) {
        m_logicalDevice->as<vk::Device>().destroyRenderPass(as<vk::RenderPass>());
    }
}

} // namespace R3

#endif // R3_VULKAN
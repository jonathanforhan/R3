#if R3_VULKAN

#include "render/CommandBuffer.hpp"

#include <vulkan/vulkan.hpp>
#include "api/Check.hpp"
#include "render/CommandPool.hpp"
#include "render/Framebuffer.hpp"
#include "render/GraphicsPipeline.hpp"
#include "render/LogicalDevice.hpp"
#include "render/RenderPass.hpp"
#include "render/Swapchain.hpp"
#include "render/VertexBuffer.hpp"

namespace R3 {

void CommandBuffer::create(const CommandBufferSpecification& spec) {
    CHECK(spec.logicalDevice != nullptr);
    CHECK(spec.swapchain != nullptr);
    CHECK(spec.commandPool != nullptr);
    m_spec = spec;

    vk::CommandBufferAllocateInfo commandBufferAllocateInfo = {
        .sType = vk::StructureType::eCommandBufferAllocateInfo,
        .pNext = nullptr,
        .commandPool = m_spec.commandPool->handle<VkCommandPool>(),
        .level = vk::CommandBufferLevel::ePrimary,
        .commandBufferCount = 1,
    };

    setHandle(m_spec.logicalDevice->as<vk::Device>().allocateCommandBuffers(commandBufferAllocateInfo).front());
}

void CommandBuffer::destroy() {
    m_spec.logicalDevice->as<vk::Device>().freeCommandBuffers(m_spec.commandPool->as<vk::CommandPool>(),
                                                              {as<vk::CommandBuffer>()});
}

void CommandBuffer::resetCommandBuffer() {
    as<vk::CommandBuffer>().reset();
}

void CommandBuffer::beginCommandBuffer() {
    vk::CommandBufferBeginInfo commandBufferBeginInfo = {
        .sType = vk::StructureType::eCommandBufferBeginInfo,
        .pNext = nullptr,
        .flags = {},
        .pInheritanceInfo = nullptr,
    };

    as<vk::CommandBuffer>().begin(commandBufferBeginInfo);
}

void CommandBuffer::endCommandBuffer() {
    as<vk::CommandBuffer>().end();
}

void CommandBuffer::beginRenderPass(const RenderPass& renderPass, const Framebuffer& framebuffer) {
    vk::ClearValue clearColor = vk::ClearColorValue(0.0f, 0.0f, 0.0f, 1.0f);

    vk::RenderPassBeginInfo renderPassBeginInfo = {
        .sType = vk::StructureType::eRenderPassBeginInfo,
        .pNext = nullptr,
        .renderPass = renderPass.handle<VkRenderPass>(),
        .framebuffer = framebuffer.handle<VkFramebuffer>(),
        .renderArea =
            {
                .offset = {0, 0},
                .extent = {m_spec.swapchain->extent().x, m_spec.swapchain->extent().y},
            },
        .clearValueCount = 1,
        .pClearValues = &clearColor,
    };

    as<vk::CommandBuffer>().beginRenderPass(renderPassBeginInfo, vk::SubpassContents::eInline);
}

void CommandBuffer::endRenderPass() {
    as<vk::CommandBuffer>().endRenderPass();
}

void CommandBuffer::bindPipeline(const GraphicsPipeline& graphicsPipeline) {
    as<vk::CommandBuffer>().bindPipeline(vk::PipelineBindPoint::eGraphics, graphicsPipeline.as<vk::Pipeline>());

    vk::Viewport viewport = {
        .x = 0.0f,
        .y = 0.0f,
        .width = static_cast<float>(m_spec.swapchain->extent().x),
        .height = static_cast<float>(m_spec.swapchain->extent().y),
        .minDepth = 0.0f,
        .maxDepth = 1.0f,
    };
    as<vk::CommandBuffer>().setViewport(0, {viewport});

    vk::Rect2D scissor = {
        .offset = {0, 0},
        .extent = {m_spec.swapchain->extent().x, m_spec.swapchain->extent().y},
    };
    as<vk::CommandBuffer>().setScissor(0, {scissor});
}

void CommandBuffer::bindVertexBuffers(const std::vector<VertexBuffer>& vertexBuffers) {
    std::vector<vk::Buffer> buffers(vertexBuffers.size());

    for (uint32 i = 0; i < vertexBuffers.size(); i++) {
        buffers[i] = vertexBuffers[i].as<vk::Buffer>();
    }

    as<vk::CommandBuffer>().bindVertexBuffers(0, {buffers}, {0});
}

} // namespace R3

#endif // R3_VULKAN
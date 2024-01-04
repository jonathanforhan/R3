#if R3_VULKAN

#include "render/CommandBuffer.hpp"

#include <vulkan/vulkan.hpp>
#include "api/Check.hpp"
#include "render/CommandPool.hpp"
#include "render/DescriptorSet.hpp"
#include "render/Fence.hpp"
#include "render/Framebuffer.hpp"
#include "render/GraphicsPipeline.hpp"
#include "render/IndexBuffer.hpp"
#include "render/LogicalDevice.hpp"
#include "render/RenderPass.hpp"
#include "render/Semaphore.hpp"
#include "render/Swapchain.hpp"
#include "render/VertexBuffer.hpp"

namespace R3 {

namespace local {

static constexpr vk::CommandBufferUsageFlags CommandBufferFlagsToVkFlags(CommandBufferUsage flags) {
    switch (flags) {
        case CommandBufferUsage::OneTimeSubmit:
            return vk::CommandBufferUsageFlagBits::eOneTimeSubmit;
        case CommandBufferUsage::RenderPassContinue:
            return vk::CommandBufferUsageFlagBits::eRenderPassContinue;
        case CommandBufferUsage::SimultaneousUse:
            return vk::CommandBufferUsageFlagBits::eSimultaneousUse;
        case CommandBufferUsage::Nil:
        default:
            return {};
    }
}

} // namespace local

std::vector<CommandBuffer> CommandBuffer::allocate(const CommandBufferSpecification& spec) {
    const vk::CommandBufferAllocateInfo commandBufferAllocateInfo = {
        .sType = vk::StructureType::eCommandBufferAllocateInfo,
        .pNext = nullptr,
        .commandPool = spec.commandPool.as<vk::CommandPool>(),
        .level = vk::CommandBufferLevel::ePrimary,
        .commandBufferCount = spec.commandBufferCount,
    };

    const auto allocatedCommandBuffers =
        spec.logicalDevice.as<vk::Device>().allocateCommandBuffers(commandBufferAllocateInfo);
    CHECK(allocatedCommandBuffers.size() == spec.commandBufferCount);

    std::vector<CommandBuffer> commandBuffers(spec.commandBufferCount);

    for (usize i = 0; i < commandBuffers.size(); i++) {
        commandBuffers[i].m_logicalDevice = &spec.logicalDevice;
        commandBuffers[i].m_swapchain = &spec.swapchain;
        commandBuffers[i].m_commandPool = &spec.commandPool;
        commandBuffers[i].setHandle(allocatedCommandBuffers[i]);
    }

    return commandBuffers;
}

void CommandBuffer::free(std::vector<CommandBuffer>& commandBuffers) {
    std::vector<vk::CommandBuffer> buffers;
    for (const auto& commandBuffer : commandBuffers) {
        buffers.push_back(commandBuffer.as<vk::CommandBuffer>());
    }

    const auto& logicalDevice = commandBuffers.front().m_logicalDevice;
    const auto& commandPool = commandBuffers.front().m_commandPool;

    logicalDevice->as<vk::Device>().freeCommandBuffers(commandPool->as<vk::CommandPool>(), buffers);
}

void CommandBuffer::free() {
    m_logicalDevice->as<vk::Device>().freeCommandBuffers(m_commandPool->as<vk::CommandPool>(),
                                                         {as<vk::CommandBuffer>()});
}

void CommandBuffer::resetCommandBuffer() const {
    as<vk::CommandBuffer>().reset();
}

void CommandBuffer::beginCommandBuffer(CommandBufferUsage usage) const {
    const vk::CommandBufferBeginInfo commandBufferBeginInfo = {
        .sType = vk::StructureType::eCommandBufferBeginInfo,
        .pNext = nullptr,
        .flags = local::CommandBufferFlagsToVkFlags(usage),
        .pInheritanceInfo = nullptr,
    };

    as<vk::CommandBuffer>().begin(commandBufferBeginInfo);
}

void CommandBuffer::endCommandBuffer() const {
    as<vk::CommandBuffer>().end();
}

void CommandBuffer::beginRenderPass(const RenderPass& renderPass, const Framebuffer& framebuffer) const {
    constexpr vk::ClearValue clearValues[] = {
        vk::ClearColorValue(0.0f, 0.0f, 0.0f, 1.0f),
        vk::ClearDepthStencilValue(1.0f, 0U),
    };

    const vk::RenderPassBeginInfo renderPassBeginInfo = {
        .sType = vk::StructureType::eRenderPassBeginInfo,
        .pNext = nullptr,
        .renderPass = renderPass.as<vk::RenderPass>(),
        .framebuffer = framebuffer.as<vk::Framebuffer>(),
        .renderArea =
            {
                .offset = {0, 0},
                .extent = {m_swapchain->extent().x, m_swapchain->extent().y},
            },
        .clearValueCount = static_cast<uint32>(std::size(clearValues)),
        .pClearValues = clearValues,
    };

    as<vk::CommandBuffer>().beginRenderPass(renderPassBeginInfo, vk::SubpassContents::eInline);
}

void CommandBuffer::endRenderPass() const {
    as<vk::CommandBuffer>().endRenderPass();
}

void CommandBuffer::bindPipeline(const GraphicsPipeline& graphicsPipeline) const {
    as<vk::CommandBuffer>().bindPipeline(vk::PipelineBindPoint::eGraphics, graphicsPipeline.as<vk::Pipeline>());

    // set dynamic viewport
    const vk::Viewport viewport = {
        .x = 0.0f,
        .y = static_cast<float>(m_swapchain->extent().y),
        .width = static_cast<float>(m_swapchain->extent().x),
        .height = -static_cast<float>(m_swapchain->extent().y),
        .minDepth = 0.0f,
        .maxDepth = 1.0f,
    };
    as<vk::CommandBuffer>().setViewport(0, {viewport});

    // set dynamic scissor
    const vk::Rect2D scissor = {
        .offset = {0, 0},
        .extent = {m_swapchain->extent().x, m_swapchain->extent().y},
    };
    as<vk::CommandBuffer>().setScissor(0, {scissor});
}

void CommandBuffer::bindVertexBuffer(const VertexBuffer& vertexBuffer) const {
    as<vk::CommandBuffer>().bindVertexBuffers(0, {vertexBuffer.as<vk::Buffer>()}, {0});
}

void CommandBuffer::bindIndexBuffer(const IndexBuffer<uint16>& indexBuffer) const {
    as<vk::CommandBuffer>().bindIndexBuffer(indexBuffer.as<vk::Buffer>(), 0, vk::IndexType::eUint16);
}

void CommandBuffer::bindIndexBuffer(const IndexBuffer<uint32>& indexBuffer) const {
    as<vk::CommandBuffer>().bindIndexBuffer(indexBuffer.as<vk::Buffer>(), 0, vk::IndexType::eUint32);
}

void CommandBuffer::bindDescriptorSet(const PipelineLayout& pipelineLayout, const DescriptorSet& descriptorSet) const {
    vk::DescriptorSet descriptors[]{descriptorSet.as<vk::DescriptorSet>()};
    as<vk::CommandBuffer>().bindDescriptorSets(
        vk::PipelineBindPoint::eGraphics, pipelineLayout.as<vk::PipelineLayout>(), 0, descriptors, {});
}

void CommandBuffer::pushConstants(const PipelineLayout& layout,
                                  ShaderStage::Flags stage,
                                  const void* data,
                                  usize size,
                                  usize offset) const {
    as<vk::CommandBuffer>().pushConstants(layout.as<vk::PipelineLayout>(),
                                          vk::ShaderStageFlags(stage),
                                          static_cast<uint32>(offset),
                                          static_cast<uint32>(size),
                                          data);
}

void CommandBuffer::submit(const CommandBufferSumbitSpecification& spec) const {
    static_assert(sizeof(NativeRenderObject) == sizeof(vk::Semaphore));

    vk::PipelineStageFlags waitStage(spec.waitStages);

    const auto commandBuffer = as<vk::CommandBuffer>();

    const vk::SubmitInfo submitInfo = {
        .sType = vk::StructureType::eSubmitInfo,
        .pNext = nullptr,
        .waitSemaphoreCount = uint32(spec.waitSemaphores.size()),
        .pWaitSemaphores = (const vk::Semaphore*)spec.waitSemaphores.data(),
        .pWaitDstStageMask = &waitStage,
        .commandBufferCount = 1,
        .pCommandBuffers = &commandBuffer,
        .signalSemaphoreCount = uint32(spec.waitSemaphores.size()),
        .pSignalSemaphores = (const vk::Semaphore*)spec.signalSemaphores.data(),
    };

    spec.fence ? m_logicalDevice->graphicsQueue().as<vk::Queue>().submit(submitInfo, spec.fence->as<vk::Fence>())
               : m_logicalDevice->graphicsQueue().as<vk::Queue>().submit(submitInfo);
}

void CommandBuffer::oneTimeSubmit() const {
    const auto vkCommandBuffer = as<vk::CommandBuffer>();
    const vk::SubmitInfo submitInfo = {
        .sType = vk::StructureType::eSubmitInfo,
        .commandBufferCount = 1,
        .pCommandBuffers = &vkCommandBuffer,
    };
    m_logicalDevice->graphicsQueue().as<vk::Queue>().submit(submitInfo);
    m_logicalDevice->graphicsQueue().as<vk::Queue>().waitIdle();
}

int32 CommandBuffer::present(const CommandBufferPresentSpecification& spec) const {
    static_assert(sizeof(NativeRenderObject) == sizeof(vk::Semaphore));

    const auto swapchain = m_swapchain->as<vk::SwapchainKHR>();

    const vk::PresentInfoKHR presentInfo = {
        .sType = vk::StructureType::ePresentInfoKHR,
        .pNext = nullptr,
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = (const vk::Semaphore*)spec.waitSemaphores.data(),
        .swapchainCount = 1,
        .pSwapchains = &swapchain,
        .pImageIndices = &spec.currentImageIndex,
        .pResults = nullptr,
    };

    // MAY THROW !
    return (int32)m_logicalDevice->presentationQueue().as<vk::Queue>().presentKHR(presentInfo);
}

} // namespace R3

#endif // R3_VULKAN
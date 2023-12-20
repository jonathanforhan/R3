#if R3_VULKAN

#include "render/CommandBuffer.hpp"

#include <array>
#include <vulkan/vulkan.hpp>
#include "api/Check.hpp"
#include "render/CommandPool.hpp"
#include "render/DescriptorSet.hpp"
#include "render/Framebuffer.hpp"
#include "render/GraphicsPipeline.hpp"
#include "render/IndexBuffer.hpp"
#include "render/LogicalDevice.hpp"
#include "render/PipelineLayout.hpp"
#include "render/RenderPass.hpp"
#include "render/Swapchain.hpp"
#include "render/VertexBuffer.hpp"

namespace R3 {

namespace local {

static constexpr vk::CommandBufferUsageFlags CommandBufferFlagsToVkFlags(CommandBufferFlags flags) {
    switch (flags) {
        case CommandBufferFlags::OneTimeSubmit:
            return vk::CommandBufferUsageFlagBits::eOneTimeSubmit;
        case CommandBufferFlags::RenderPassContinue:
            return vk::CommandBufferUsageFlagBits::eRenderPassContinue;
        case CommandBufferFlags::SimultaneousUse:
            return vk::CommandBufferUsageFlagBits::eSimultaneousUse;
        case CommandBufferFlags::Nil:
        default:
            return {};
    }
}

} // namespace local

std::vector<CommandBuffer> CommandBuffer::allocate(const CommandBufferSpecification& spec) {
    const vk::CommandBufferAllocateInfo commandBufferAllocateInfo = {
        .sType = vk::StructureType::eCommandBufferAllocateInfo,
        .pNext = nullptr,
        .commandPool = spec.commandPool->as<vk::CommandPool>(),
        .level = vk::CommandBufferLevel::ePrimary,
        .commandBufferCount = spec.commandBufferCount,
    };

    const auto allocatedCommandBuffers =
        spec.logicalDevice->as<vk::Device>().allocateCommandBuffers(commandBufferAllocateInfo);
    CHECK(allocatedCommandBuffers.size() == spec.commandBufferCount);

    std::vector<CommandBuffer> commandBuffers(spec.commandBufferCount);

    for (usize i = 0; i < commandBuffers.size(); i++) {
        commandBuffers[i].m_spec = spec;
        commandBuffers[i].setHandle(allocatedCommandBuffers[i]);
    }

    return commandBuffers;
}

void CommandBuffer::free(std::vector<CommandBuffer>& commandBuffers) {
    std::vector<vk::CommandBuffer> buffers;
    for (const auto& commandBuffer : commandBuffers) {
        buffers.push_back(commandBuffer.as<vk::CommandBuffer>());
    }
    const auto& spec = commandBuffers.front().m_spec;
    spec.logicalDevice->as<vk::Device>().freeCommandBuffers(spec.commandPool->as<vk::CommandPool>(), buffers);
}

void CommandBuffer::free() {
    m_spec.logicalDevice->as<vk::Device>().freeCommandBuffers(m_spec.commandPool->as<vk::CommandPool>(),
                                                              {as<vk::CommandBuffer>()});
}

void CommandBuffer::resetCommandBuffer() const {
    as<vk::CommandBuffer>().reset();
}

void CommandBuffer::beginCommandBuffer(CommandBufferFlags flags) const {
    const vk::CommandBufferBeginInfo commandBufferBeginInfo = {
        .sType = vk::StructureType::eCommandBufferBeginInfo,
        .pNext = nullptr,
        .flags = local::CommandBufferFlagsToVkFlags(flags),
        .pInheritanceInfo = nullptr,
    };

    as<vk::CommandBuffer>().begin(commandBufferBeginInfo);
}

void CommandBuffer::endCommandBuffer() const {
    as<vk::CommandBuffer>().end();
}

void CommandBuffer::beginRenderPass(const RenderPass& renderPass, const Framebuffer& framebuffer) const {
    constexpr std::array<vk::ClearValue, 2> clearValues = {
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
                .extent = {m_spec.swapchain->extent().x, m_spec.swapchain->extent().y},
            },
        .clearValueCount = static_cast<uint32>(clearValues.size()),
        .pClearValues = clearValues.data(),
    };

    as<vk::CommandBuffer>().beginRenderPass(renderPassBeginInfo, vk::SubpassContents::eInline);
}

void CommandBuffer::endRenderPass() const {
    as<vk::CommandBuffer>().endRenderPass();
}

void CommandBuffer::bindPipeline(const GraphicsPipeline& graphicsPipeline) const {
    as<vk::CommandBuffer>().bindPipeline(vk::PipelineBindPoint::eGraphics, graphicsPipeline.as<vk::Pipeline>());

    const vk::Viewport viewport = {
        .x = 0.0f,
        .y = static_cast<float>(m_spec.swapchain->extent().y),
        .width = static_cast<float>(m_spec.swapchain->extent().x),
        .height = -static_cast<float>(m_spec.swapchain->extent().y),
        .minDepth = 0.0f,
        .maxDepth = 1.0f,
    };
    as<vk::CommandBuffer>().setViewport(0, {viewport});

    const vk::Rect2D scissor = {
        .offset = {0, 0},
        .extent = {m_spec.swapchain->extent().x, m_spec.swapchain->extent().y},
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

void CommandBuffer::oneTimeSubmit() const {
    const auto vkCommandBuffer = as<vk::CommandBuffer>();
    const vk::SubmitInfo submitInfo = {
        .sType = vk::StructureType::eSubmitInfo,
        .commandBufferCount = 1,
        .pCommandBuffers = &vkCommandBuffer,
    };
    m_spec.logicalDevice->graphicsQueue().as<vk::Queue>().submit(submitInfo);
    m_spec.logicalDevice->graphicsQueue().as<vk::Queue>().waitIdle();
}

} // namespace R3

#endif // R3_VULKAN
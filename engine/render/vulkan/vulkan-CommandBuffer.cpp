#include "vulkan-CommandBuffer.hpp"

#include <exception>
#include <memory>
#include <new>
#include <utility>
#include <vector>
#include <vulkan/vulkan.h>
#include "api/Assert.hpp"
#include "api/Exception.hpp"
#include "api/Types.hpp"
#include "core/Engine.hpp"
#include "core/Log.hpp"
#include "vulkan-Check.hpp"
#include "vulkan-RenderContext.hpp"

namespace R3::vulkan {

CommandBuffer::CommandBuffer(VkCommandBuffer commandBuffer, std::shared_ptr<VkCommandPool> pool)
    : m_device(VK_NULL_HANDLE),
      m_commandBuffer(commandBuffer),
      m_pool(std::move(pool)) {}

std::vector<CommandBuffer> CommandBuffer::allocate(RenderContext& ctx,
                                                   uint32 queueFamilyIndex,
                                                   VkCommandPoolCreateFlags flags,
                                                   uint32 count) {
    // Create shared command pool
    std::shared_ptr<VkCommandPool> pool(new VkCommandPool, [device = ctx.device()](VkCommandPool* p) noexcept {
        if (device && p) {
            vkDestroyCommandPool(device, *p, nullptr);
        }
    });

    const VkCommandPoolCreateInfo poolInfo = {
        .sType            = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .pNext            = nullptr,
        .flags            = flags,
        .queueFamilyIndex = queueFamilyIndex,
    };
    VK_CHECK(vkCreateCommandPool(ctx.device(), &poolInfo, nullptr, pool.get()));

    // Allocate command buffers from the pool
    const VkCommandBufferAllocateInfo allocInfo = {
        .sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .pNext              = nullptr,
        .commandPool        = *pool,
        .level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = count,
    };

    std::vector<VkCommandBuffer> vkCommandBuffers(count);
    VK_CHECK(vkAllocateCommandBuffers(ctx.device(), &allocInfo, vkCommandBuffers.data()));

    // Wrap each VkCommandBuffer in our CommandBuffer class
    std::vector<CommandBuffer> commandBuffers;
    commandBuffers.resize(count);

    for (uint32 i = 0; i < count; ++i) {
        commandBuffers[i] = std::move(CommandBuffer(vkCommandBuffers[i], pool));
    }

    return commandBuffers;
}

void CommandBuffer::begin(VkCommandBufferUsageFlags flags) {
    R3_ASSERT(!m_isRecording && "CommandBuffer is already recording!");

    const VkCommandBufferBeginInfo beginInfo = {
        .sType            = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .pNext            = nullptr,
        .flags            = flags,
        .pInheritanceInfo = nullptr,
    };

    VK_CHECK(vkBeginCommandBuffer(m_commandBuffer, &beginInfo));
    m_isRecording = true;
}

void CommandBuffer::end() {
    R3_ASSERT(m_isRecording && "CommandBuffer is not recording!");

    VK_CHECK(vkEndCommandBuffer(m_commandBuffer));
    m_isRecording = false;
}

void CommandBuffer::reset(VkCommandBufferResetFlags flags) {
    VK_CHECK(vkResetCommandBuffer(m_commandBuffer, flags));
    m_isRecording = false;
}

void CommandBuffer::beginRendering(const VkRenderingInfo& beginInfo) {
    R3_ASSERT(m_isRecording && "CommandBuffer must be recording!");
    vkCmdBeginRendering(m_commandBuffer, &beginInfo);
}

void CommandBuffer::endRendering() {
    R3_ASSERT(m_isRecording && "CommandBuffer must be recording!");
    vkCmdEndRendering(m_commandBuffer);
}

void CommandBuffer::bindGraphicsPipeline(VkPipeline pipeline) {
    R3_ASSERT(m_isRecording && "CommandBuffer must be recording!");
    vkCmdBindPipeline(m_commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
}

void CommandBuffer::bindComputePipeline(VkPipeline pipeline) {
    R3_ASSERT(m_isRecording && "CommandBuffer must be recording!");
    vkCmdBindPipeline(m_commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, pipeline);
}

void CommandBuffer::bindDescriptorSets(VkPipelineBindPoint bindPoint,
                                       VkPipelineLayout layout,
                                       uint32 firstSet,
                                       std::span<const VkDescriptorSet> descriptorSets,
                                       std::span<const uint32> dynamicOffsets) {
    R3_ASSERT(m_isRecording && "CommandBuffer must be recording!");

    vkCmdBindDescriptorSets(m_commandBuffer,
                            bindPoint,
                            layout,
                            firstSet,
                            static_cast<uint32>(descriptorSets.size()),
                            descriptorSets.data(),
                            static_cast<uint32>(dynamicOffsets.size()),
                            dynamicOffsets.data());
}

void CommandBuffer::bindVertexBuffers(uint32 firstBinding,
                                      std::span<const VkBuffer> buffers,
                                      std::span<const VkDeviceSize> offsets) {
    R3_ASSERT(m_isRecording && "CommandBuffer must be recording!");
    R3_ASSERT(buffers.size() == offsets.size() && "Buffer and offset counts must match!");

    vkCmdBindVertexBuffers(
        m_commandBuffer, firstBinding, static_cast<uint32>(buffers.size()), buffers.data(), offsets.data());
}

void CommandBuffer::bindIndexBuffer(VkBuffer buffer, VkDeviceSize offset, VkIndexType indexType) {
    R3_ASSERT(m_isRecording && "CommandBuffer must be recording!");
    vkCmdBindIndexBuffer(m_commandBuffer, buffer, offset, indexType);
}

void CommandBuffer::draw(uint32 vertexCount, uint32 instanceCount, uint32 firstVertex, uint32 firstInstance) {
    R3_ASSERT(m_isRecording && "CommandBuffer must be recording!");
    vkCmdDraw(m_commandBuffer, vertexCount, instanceCount, firstVertex, firstInstance);
}

void CommandBuffer::drawIndexed(uint32 indexCount,
                                uint32 instanceCount,
                                uint32 firstIndex,
                                int32 vertexOffset,
                                uint32 firstInstance) {
    R3_ASSERT(m_isRecording && "CommandBuffer must be recording!");
    vkCmdDrawIndexed(m_commandBuffer, indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
}

void CommandBuffer::dispatch(uint32 groupCountX, uint32 groupCountY, uint32 groupCountZ) {
    R3_ASSERT(m_isRecording && "CommandBuffer must be recording!");
    vkCmdDispatch(m_commandBuffer, groupCountX, groupCountY, groupCountZ);
}

void CommandBuffer::setViewport(const VkViewport& viewport) {
    R3_ASSERT(m_isRecording && "CommandBuffer must be recording!");
    vkCmdSetViewport(m_commandBuffer, 0, 1, &viewport);
}

void CommandBuffer::setScissor(const VkRect2D& scissor) {
    R3_ASSERT(m_isRecording && "CommandBuffer must be recording!");
    vkCmdSetScissor(m_commandBuffer, 0, 1, &scissor);
}

void CommandBuffer::setCullMode(VkCullModeFlags cullMode) {
    R3_ASSERT(m_isRecording && "CommandBuffer must be recording!");
    vkCmdSetCullMode(m_commandBuffer, cullMode);
}

void CommandBuffer::setLineWidth(float lineWidth) {
    R3_ASSERT(m_isRecording && "CommandBuffer must be recording!");
    vkCmdSetLineWidth(m_commandBuffer, lineWidth);
}

void CommandBuffer::setFrontFace(VkFrontFace frontFace) {
    R3_ASSERT(m_isRecording && "CommandBuffer must be recording!");
    vkCmdSetFrontFace(m_commandBuffer, frontFace);
}

void CommandBuffer::setDepthTestEnable(bool enable) {
    R3_ASSERT(m_isRecording && "CommandBuffer must be recording!");
    vkCmdSetDepthTestEnable(m_commandBuffer, enable ? VK_TRUE : VK_FALSE);
}

void CommandBuffer::transitionImageLayout(const VkImageMemoryBarrier2& imageMemoryBarrier) {
    R3_ASSERT(m_isRecording && "CommandBuffer must be recording!");
    const VkDependencyInfo dependencyInfo = {
        .sType                    = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
        .pNext                    = nullptr,
        .dependencyFlags          = 0,
        .memoryBarrierCount       = 0,
        .pMemoryBarriers          = nullptr,
        .bufferMemoryBarrierCount = 0,
        .pBufferMemoryBarriers    = nullptr,
        .imageMemoryBarrierCount  = 1,
        .pImageMemoryBarriers     = &imageMemoryBarrier,
    };
    vkCmdPipelineBarrier2(m_commandBuffer, &dependencyInfo);
}

void CommandBuffer::pipelineBarrier(VkPipelineStageFlags srcStage,
                                    VkPipelineStageFlags dstStage,
                                    VkDependencyFlags dependencyFlags,
                                    std::span<const VkMemoryBarrier> memoryBarriers,
                                    std::span<const VkBufferMemoryBarrier> bufferBarriers,
                                    std::span<const VkImageMemoryBarrier> imageBarriers) {
    R3_ASSERT(m_isRecording && "CommandBuffer must be recording!");

    vkCmdPipelineBarrier(m_commandBuffer,
                         srcStage,
                         dstStage,
                         dependencyFlags,
                         static_cast<uint32>(memoryBarriers.size()),
                         memoryBarriers.data(),
                         static_cast<uint32>(bufferBarriers.size()),
                         bufferBarriers.data(),
                         static_cast<uint32>(imageBarriers.size()),
                         imageBarriers.data());
}

void CommandBuffer::copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, std::span<const VkBufferCopy> regions) {
    R3_ASSERT(m_isRecording && "CommandBuffer must be recording!");
    vkCmdCopyBuffer(m_commandBuffer, srcBuffer, dstBuffer, static_cast<uint32>(regions.size()), regions.data());
}

void CommandBuffer::copyBufferToImage(VkBuffer srcBuffer,
                                      VkImage dstImage,
                                      VkImageLayout imageLayout,
                                      std::span<const VkBufferImageCopy> regions) {
    R3_ASSERT(m_isRecording && "CommandBuffer must be recording!");
    vkCmdCopyBufferToImage(
        m_commandBuffer, srcBuffer, dstImage, imageLayout, static_cast<uint32>(regions.size()), regions.data());
}

void CommandBuffer::copyImageToBuffer(VkImage srcImage,
                                      VkImageLayout imageLayout,
                                      VkBuffer dstBuffer,
                                      std::span<const VkBufferImageCopy> regions) {
    R3_ASSERT(m_isRecording && "CommandBuffer must be recording!");
    vkCmdCopyImageToBuffer(
        m_commandBuffer, srcImage, imageLayout, dstBuffer, static_cast<uint32>(regions.size()), regions.data());
}

void CommandBuffer::blitImage(VkImage srcImage,
                              VkImageLayout srcImageLayout,
                              VkImage dstImage,
                              VkImageLayout dstImageLayout,
                              std::span<const VkImageBlit> regions,
                              VkFilter filter) {
    R3_ASSERT(m_isRecording && "CommandBuffer must be recording!");
    vkCmdBlitImage(m_commandBuffer,
                   srcImage,
                   srcImageLayout,
                   dstImage,
                   dstImageLayout,
                   static_cast<uint32>(regions.size()),
                   regions.data(),
                   filter);
}

void CommandBuffer::pushConstants(VkPipelineLayout layout,
                                  VkShaderStageFlags stageFlags,
                                  uint32 offset,
                                  uint32 size,
                                  const void* values) {
    R3_ASSERT(m_isRecording && "CommandBuffer must be recording!");
    vkCmdPushConstants(m_commandBuffer, layout, stageFlags, offset, size, values);
}

void CommandBuffer::submit(VkQueue queue,
                           std::span<const VkSemaphore> waitSemaphores,
                           std::span<const VkPipelineStageFlags> waitStages,
                           std::span<const VkSemaphore> signalSemaphores,
                           VkFence fence) {
    R3_ASSERT(!m_isRecording && "CommandBuffer must be ended before submission!");
    R3_ASSERT(waitSemaphores.size() == waitStages.size() && "Wait semaphores and stages must match!");

    const VkSubmitInfo submitInfo = {
        .sType                = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .pNext                = nullptr,
        .waitSemaphoreCount   = static_cast<uint32>(waitSemaphores.size()),
        .pWaitSemaphores      = waitSemaphores.data(),
        .pWaitDstStageMask    = waitStages.data(),
        .commandBufferCount   = 1,
        .pCommandBuffers      = &m_commandBuffer,
        .signalSemaphoreCount = static_cast<uint32>(signalSemaphores.size()),
        .pSignalSemaphores    = signalSemaphores.data(),
    };

    VK_CHECK(vkQueueSubmit(queue, 1, &submitInfo, fence));
}

void CommandBuffer::submitSync(VkQueue queue) {
    R3_ASSERT(!m_isRecording && "CommandBuffer must be ended before submission!");

    if (queue == VK_NULL_HANDLE) {
        RenderContext& ctx = GEngine()->RenderContext<RenderContext>();
        queue              = ctx.graphicsQueue();
    }

    const VkSubmitInfo submitInfo = {
        .sType                = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .pNext                = nullptr,
        .waitSemaphoreCount   = 0,
        .pWaitSemaphores      = nullptr,
        .pWaitDstStageMask    = nullptr,
        .commandBufferCount   = 1,
        .pCommandBuffers      = &m_commandBuffer,
        .signalSemaphoreCount = 0,
        .pSignalSemaphores    = nullptr,
    };

    VK_CHECK(vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE));

    vkQueueWaitIdle(queue);
}

} // namespace R3::vulkan
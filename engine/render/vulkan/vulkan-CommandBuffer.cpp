#include "vulkan-CommandBuffer.hpp"

#include <memory>
#include <new>
#include <span>
#include <utility>
#include <vector>
#include <vulkan/vulkan.h>
#include "api/Assert.hpp"
#include "api/Types.hpp"
#include "render/Buffer.hpp"
#include "render/Image.hpp"
#include "vulkan-Check.hpp"
#include "vulkan-RenderContext.hpp"
#include "vulkan-Translation.hpp"

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
        .flags            = flags,
        .queueFamilyIndex = queueFamilyIndex,
    };
    VK_CHECK(vkCreateCommandPool(ctx.device(), &poolInfo, nullptr, pool.get()));

    // Allocate command buffers from the pool
    const VkCommandBufferAllocateInfo allocInfo = {
        .sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
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
    R3_ASSERT(!m_isRecording, "CommandBuffer is already recording!");
    const VkCommandBufferBeginInfo beginInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .flags = flags,
    };
    VK_CHECK(vkBeginCommandBuffer(m_commandBuffer, &beginInfo));
    m_isRecording = true;
}

void CommandBuffer::end() {
    R3_ASSERT(m_isRecording, "CommandBuffer is not recording!");
    VK_CHECK(vkEndCommandBuffer(m_commandBuffer));
    m_isRecording = false;
}

void CommandBuffer::reset(VkCommandBufferResetFlags flags) {
    VK_CHECK(vkResetCommandBuffer(m_commandBuffer, flags));
    m_isRecording = false;
}

void CommandBuffer::beginRendering(const VkRenderingInfo& beginInfo) {
    R3_ASSERT(m_isRecording, "CommandBuffer must be recording!");
    vkCmdBeginRendering(m_commandBuffer, &beginInfo);
}

void CommandBuffer::endRendering() {
    R3_ASSERT(m_isRecording, "CommandBuffer must be recording!");
    vkCmdEndRendering(m_commandBuffer);
}

void CommandBuffer::bindGraphicsPipeline(VkPipeline pipeline) {
    R3_ASSERT(m_isRecording, "CommandBuffer must be recording!");
    vkCmdBindPipeline(m_commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
}

void CommandBuffer::bindComputePipeline(VkPipeline pipeline) {
    R3_ASSERT(m_isRecording, "CommandBuffer must be recording!");
    vkCmdBindPipeline(m_commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, pipeline);
}

void CommandBuffer::bindDescriptorSets(const VkBindDescriptorSetsInfo& bindDescriptorSetsInfo) {
    R3_ASSERT(m_isRecording, "CommandBuffer must be recording!");
    // vkCmdBindDescriptorSets2(m_commandBuffer, &bindDescriptorSetsInfo); // FIXME not supported
    const VkPipelineBindPoint pipelineBindPoint = bindDescriptorSetsInfo.stageFlags & VK_SHADER_STAGE_COMPUTE_BIT
                                                      ? VK_PIPELINE_BIND_POINT_COMPUTE
                                                      : VK_PIPELINE_BIND_POINT_GRAPHICS;
    // ^^ not perfect but works for now
    vkCmdBindDescriptorSets(m_commandBuffer,
                            pipelineBindPoint,
                            bindDescriptorSetsInfo.layout,
                            bindDescriptorSetsInfo.firstSet,
                            bindDescriptorSetsInfo.descriptorSetCount,
                            bindDescriptorSetsInfo.pDescriptorSets,
                            bindDescriptorSetsInfo.dynamicOffsetCount,
                            bindDescriptorSetsInfo.pDynamicOffsets);
}

void CommandBuffer::bindVertexBuffers(uint32 firstBinding,
                                      std::span<const VkBuffer> buffers,
                                      std::span<const VkDeviceSize> offsets) {
    R3_ASSERT(m_isRecording, "CommandBuffer must be recording!");
    R3_ASSERT(buffers.size() == offsets.size(), "Buffer and offset counts must match!");
    vkCmdBindVertexBuffers(
        m_commandBuffer, firstBinding, static_cast<uint32>(buffers.size()), buffers.data(), offsets.data());
}

void CommandBuffer::bindIndexBuffer(VkBuffer buffer, VkDeviceSize offset, VkIndexType indexType) {
    R3_ASSERT(m_isRecording, "CommandBuffer must be recording!");
    vkCmdBindIndexBuffer(m_commandBuffer, buffer, offset, indexType);
}

void CommandBuffer::draw(uint32 vertexCount, uint32 instanceCount, uint32 firstVertex, uint32 firstInstance) {
    R3_ASSERT(m_isRecording, "CommandBuffer must be recording!");
    vkCmdDraw(m_commandBuffer, vertexCount, instanceCount, firstVertex, firstInstance);
}

void CommandBuffer::drawIndexed(uint32 indexCount,
                                uint32 instanceCount,
                                uint32 firstIndex,
                                int32 vertexOffset,
                                uint32 firstInstance) {
    R3_ASSERT(m_isRecording, "CommandBuffer must be recording!");
    vkCmdDrawIndexed(m_commandBuffer, indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
}

void CommandBuffer::dispatch(uint32 groupCountX, uint32 groupCountY, uint32 groupCountZ) {
    R3_ASSERT(m_isRecording, "CommandBuffer must be recording!");
    vkCmdDispatch(m_commandBuffer, groupCountX, groupCountY, groupCountZ);
}

void CommandBuffer::setViewport(const VkViewport& viewport) {
    R3_ASSERT(m_isRecording, "CommandBuffer must be recording!");
    vkCmdSetViewport(m_commandBuffer, 0, 1, &viewport);
}

void CommandBuffer::setScissor(const VkRect2D& scissor) {
    R3_ASSERT(m_isRecording, "CommandBuffer must be recording!");
    vkCmdSetScissor(m_commandBuffer, 0, 1, &scissor);
}

void CommandBuffer::setCullMode(VkCullModeFlags cullMode) {
    R3_ASSERT(m_isRecording, "CommandBuffer must be recording!");
    vkCmdSetCullMode(m_commandBuffer, cullMode);
}

void CommandBuffer::setLineWidth(float lineWidth) {
    R3_ASSERT(m_isRecording, "CommandBuffer must be recording!");
    vkCmdSetLineWidth(m_commandBuffer, lineWidth);
}

void CommandBuffer::setFrontFace(VkFrontFace frontFace) {
    R3_ASSERT(m_isRecording, "CommandBuffer must be recording!");
    vkCmdSetFrontFace(m_commandBuffer, frontFace);
}

void CommandBuffer::setDepthTestEnable(bool enable) {
    R3_ASSERT(m_isRecording, "CommandBuffer must be recording!");
    vkCmdSetDepthTestEnable(m_commandBuffer, enable ? VK_TRUE : VK_FALSE);
}

void CommandBuffer::pushConstants(const VkPushConstantsInfo& pushConstantsInfo) {
    R3_ASSERT(m_isRecording, "CommandBuffer must be recording!");
    vkCmdPushConstants2(m_commandBuffer, &pushConstantsInfo);
}

void CommandBuffer::copyBuffer(const VkCopyBufferInfo2& copyInfo) {
    R3_ASSERT(m_isRecording, "CommandBuffer must be recording!");
    vkCmdCopyBuffer2(m_commandBuffer, &copyInfo);
}

void CommandBuffer::copyImage(const VkCopyImageInfo2& copyInfo) {
    R3_ASSERT(m_isRecording, "CommandBuffer must be recording!");
    vkCmdCopyImage2(m_commandBuffer, &copyInfo);
}

void CommandBuffer::copyBufferToImage(const VkCopyBufferToImageInfo2& copyInfo) {
    R3_ASSERT(m_isRecording, "CommandBuffer must be recording!");
    vkCmdCopyBufferToImage2(m_commandBuffer, &copyInfo);
}

void CommandBuffer::copyImageToBuffer(const VkCopyImageToBufferInfo2& copyInfo) {
    R3_ASSERT(m_isRecording, "CommandBuffer must be recording!");
    vkCmdCopyImageToBuffer2(m_commandBuffer, &copyInfo);
}

void CommandBuffer::blitImage(const VkBlitImageInfo2& blitInfo) {
    R3_ASSERT(m_isRecording, "CommandBuffer must be recording!");
    vkCmdBlitImage2(m_commandBuffer, &blitInfo);
}

void CommandBuffer::resolveImage(const VkResolveImageInfo2& resolveInfo) {
    R3_ASSERT(m_isRecording, "CommandBuffer must be recording!");
    vkCmdResolveImage2(m_commandBuffer, &resolveInfo);
}

void CommandBuffer::copyBuffer(const Buffer& src, Buffer& dst) {
    R3_ASSERT(m_isRecording, "CommandBuffer must be recording!");
    R3_ASSERT(src.size() == dst.size(), "Source and destination buffer sizes must match!");
    const VkBufferCopy copy = {.srcOffset = 0, .dstOffset = 0, .size = src.size()};
    vkCmdCopyBuffer(m_commandBuffer, src.bufferHandle(), dst.bufferHandle(), 1, &copy);
}

void CommandBuffer::copyBuffer(const Buffer& src, usize srcOffset, Buffer& dst, usize dstOffset, usize size) {
    R3_ASSERT(m_isRecording, "CommandBuffer must be recording!");
    const VkBufferCopy copy = {.srcOffset = srcOffset, .dstOffset = dstOffset, .size = size};
    vkCmdCopyBuffer(m_commandBuffer, src.bufferHandle(), dst.bufferHandle(), 1, &copy);
}

void CommandBuffer::copyImage(const Image& src, Image& dst) {
    R3_ASSERT(m_isRecording, "CommandBuffer must be recording!");
    R3_ASSERT(src.extent() == dst.extent(), "Source and destination image extents must match!");
    const VkImageCopy copy = {
        .srcSubresource =
            {
                .aspectMask     = TO_VK_IMAGE_ASPECT(src.usage()),
                .mipLevel       = src.mipLevels(),
                .baseArrayLayer = 0,
                .layerCount     = src.layerCount(),
            },
        .srcOffset = {0, 0, 0},
        .dstSubresource =
            {
                .aspectMask     = TO_VK_IMAGE_ASPECT(dst.usage()),
                .mipLevel       = dst.mipLevels(),
                .baseArrayLayer = 0,
                .layerCount     = dst.layerCount(),
            },
        .dstOffset = {0, 0, 0},
        .extent    = {(uint32)dst.extent().x, (uint32)dst.extent().y, (uint32)dst.extent().z},
    };
    vkCmdCopyImage(m_commandBuffer,
                   src.imageHandle(),
                   VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                   dst.imageHandle(),
                   VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                   1,
                   &copy);
}

void CommandBuffer::copyImage(const Image& src, usize3 srcOffset, Image& dst, usize3 dstOffset, usize3 extent) {
    R3_ASSERT(m_isRecording, "CommandBuffer must be recording!");
    const VkImageCopy copy = {
        .srcSubresource =
            {
                .aspectMask     = TO_VK_IMAGE_ASPECT(src.usage()),
                .mipLevel       = 0,
                .baseArrayLayer = 0,
                .layerCount     = src.layerCount(),
            },
        .srcOffset = {(int32)srcOffset.x, (int32)srcOffset.y, (int32)srcOffset.z},
        .dstSubresource =
            {
                .aspectMask     = TO_VK_IMAGE_ASPECT(dst.usage()),
                .mipLevel       = 0,
                .baseArrayLayer = 0,
                .layerCount     = dst.layerCount(),
            },
        .dstOffset = {(int32)dstOffset.x, (int32)dstOffset.y, (int32)dstOffset.z},
        .extent    = {(uint32)extent.x, (uint32)extent.y, (uint32)extent.z},
    };
    vkCmdCopyImage(m_commandBuffer,
                   src.imageHandle(),
                   VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                   dst.imageHandle(),
                   VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                   1,
                   &copy);
}

void CommandBuffer::copyBufferToImage(const Buffer& src, Image& dst) {
    R3_ASSERT(m_isRecording, "CommandBuffer must be recording!");
    copyBufferToImage(src, 0, dst, {0, 0, 0}, dst.extent());
}

void CommandBuffer::copyBufferToImage(const Buffer& src,
                                      usize srcOffset,
                                      Image& dst,
                                      usize3 dstOffset,
                                      usize3 dstExtent) {
    R3_ASSERT(m_isRecording, "CommandBuffer must be recording!");

    const VkImageMemoryBarrier2 barrier = {
        .sType               = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
        .srcStageMask        = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
        .srcAccessMask       = VK_ACCESS_NONE,
        .dstStageMask        = VK_PIPELINE_STAGE_TRANSFER_BIT,
        .dstAccessMask       = VK_ACCESS_TRANSFER_WRITE_BIT,
        .oldLayout           = VK_IMAGE_LAYOUT_UNDEFINED,
        .newLayout           = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .image               = dst.imageHandle(),
        .subresourceRange =
            {
                .aspectMask     = TO_VK_IMAGE_ASPECT(dst.usage()),
                .baseMipLevel   = 0,
                .levelCount     = dst.mipLevels(),
                .baseArrayLayer = 0,
                .layerCount     = dst.layerCount(),
            },
    };
    pipelineBarrier({
        .sType                   = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
        .imageMemoryBarrierCount = 1,
        .pImageMemoryBarriers    = &barrier,
    });

    const uint32 faceSize = (uint32)(dst.extent().x * dst.extent().y * dst.extent().z) * dst.bytesPerPixel();

    for (uint32 face = 0; face < dst.layerCount(); ++face) {
        const VkBufferImageCopy copy = {
            .bufferOffset      = srcOffset + (face * faceSize),
            .bufferRowLength   = 0,
            .bufferImageHeight = 0,
            .imageSubresource =
                {
                    .aspectMask     = TO_VK_IMAGE_ASPECT(dst.usage()),
                    .mipLevel       = 0,
                    .baseArrayLayer = face,
                    .layerCount     = 1,
                },
            .imageOffset = {(int32)dstOffset.x, (int32)dstOffset.y, (int32)dstOffset.z},
            .imageExtent = {(uint32)dstExtent.x, (uint32)dstExtent.y, (uint32)dstExtent.z},
        };
        vkCmdCopyBufferToImage(
            m_commandBuffer, src.bufferHandle(), dst.imageHandle(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copy);
    }
}

void CommandBuffer::copyImageToBuffer(const Image& src, Buffer& dst) {
    R3_ASSERT(m_isRecording, "CommandBuffer must be recording!");
    const VkBufferImageCopy copy = {
        .bufferOffset      = 0,
        .bufferRowLength   = 0,
        .bufferImageHeight = 0,
        .imageSubresource =
            {
                .aspectMask     = TO_VK_IMAGE_ASPECT(src.usage()),
                .mipLevel       = 0,
                .baseArrayLayer = 0,
                .layerCount     = src.layerCount(),
            },
        .imageOffset = {0, 0, 0},
        .imageExtent = {(uint32)src.extent().x, (uint32)src.extent().y, (uint32)src.extent().z},
    };
    vkCmdCopyImageToBuffer(
        m_commandBuffer, src.imageHandle(), VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, dst.bufferHandle(), 1, &copy);
}

void CommandBuffer::copyImageToBuffer(const Image& src,
                                      usize3 srcOffset,
                                      usize3 srcExtent,
                                      Buffer& dst,
                                      usize dstOffset) {
    R3_ASSERT(m_isRecording, "CommandBuffer must be recording!");
    const VkBufferImageCopy copy = {
        .bufferOffset      = dstOffset,
        .bufferRowLength   = 0,
        .bufferImageHeight = 0,
        .imageSubresource =
            {
                .aspectMask     = TO_VK_IMAGE_ASPECT(src.usage()),
                .mipLevel       = 0,
                .baseArrayLayer = 0,
                .layerCount     = src.layerCount(),
            },
        .imageOffset = {(int32)srcOffset.x, (int32)srcOffset.y, (int32)srcOffset.z},
        .imageExtent = {(uint32)srcExtent.x, (uint32)srcExtent.y, (uint32)srcExtent.z},
    };
    vkCmdCopyImageToBuffer(
        m_commandBuffer, src.imageHandle(), VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, dst.bufferHandle(), 1, &copy);
}

void CommandBuffer::pipelineBarrier(VkDependencyInfo dependencyInfo) {
    R3_ASSERT(m_isRecording, "CommandBuffer must be recording!");
    vkCmdPipelineBarrier2(m_commandBuffer, &dependencyInfo);
}

} // namespace R3::vulkan
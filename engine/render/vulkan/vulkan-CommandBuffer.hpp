#pragma once

#include <memory>
#include <span>
#include <vector>
#include <vulkan/vulkan.h>
#include "engine/api/Api.hpp"
#include "engine/api/Class.hpp"
#include "engine/api/Types.hpp"
#include "vulkan-Fwd.hpp"

namespace R3::vulkan {

class R3_API CommandBuffer {
public:
    R3_CTOR_DEFAULT(CommandBuffer);

private:
    /// Private construct R3_APIor - use allocate() instead
    CommandBuffer(VkCommandBuffer commandBuffer, std::shared_ptr<VkCommandPool> pool);

public:
    /// Static factory method to allocate command buffers from a pool
    [[nodiscard]] static std::vector<CommandBuffer> allocate(RenderContext& ctx,
                                                             uint32 queueFamilyIndex,
                                                             VkCommandPoolCreateFlags flags,
                                                             uint32 count = 1);

    /// Basic command buffer lifecycle
    void begin(VkCommandBufferUsageFlags flags = 0);
    void end();
    void reset(VkCommandBufferResetFlags flags = 0);

    /// Render pass commands
    void beginRendering(const VkRenderingInfo& beginInfo);
    void endRendering();

    /// Pipeline binding
    void bindGraphicsPipeline(VkPipeline pipeline);
    void bindComputePipeline(VkPipeline pipeline);
    void bindDescriptorSets(VkPipelineBindPoint bindPoint,
                            VkPipelineLayout layout,
                            uint32 firstSet,
                            std::span<const VkDescriptorSet> descriptorSets,
                            std::span<const uint32> dynamicOffsets = {});

    /// Vertex/Index buffers
    void bindVertexBuffers(uint32 firstBinding,
                           std::span<const VkBuffer> buffers,
                           std::span<const VkDeviceSize> offsets);
    void bindIndexBuffer(VkBuffer buffer, VkDeviceSize offset, VkIndexType indexType);

    /// Drawing commands
    void draw(uint32 vertexCount, uint32 instanceCount = 1, uint32 firstVertex = 0, uint32 firstInstance = 0);
    void drawIndexed(uint32 indexCount,
                     uint32 instanceCount = 1,
                     uint32 firstIndex    = 0,
                     int32 vertexOffset   = 0,
                     uint32 firstInstance = 0);
    void dispatch(uint32 groupCountX, uint32 groupCountY, uint32 groupCountZ);

    /// Dynamic state
    void setViewport(const VkViewport& viewport);
    void setScissor(const VkRect2D& scissor);
    void setCullMode(VkCullModeFlags cullMode);
    void setLineWidth(float lineWidth);
    void setFrontFace(VkFrontFace frontFace);
    void setDepthTestEnable(bool enable);

    /// Resource transitions
    void transitionImageLayout(const VkImageMemoryBarrier2& imageMemoryBarrier);

    void pipelineBarrier(VkPipelineStageFlags srcStage,
                         VkPipelineStageFlags dstStage,
                         VkDependencyFlags dependencyFlags,
                         std::span<const VkMemoryBarrier> memoryBarriers       = {},
                         std::span<const VkBufferMemoryBarrier> bufferBarriers = {},
                         std::span<const VkImageMemoryBarrier> imageBarriers   = {});

    /// Copy operations
    void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, std::span<const VkBufferCopy> regions);
    void copyBufferToImage(VkBuffer srcBuffer,
                           VkImage dstImage,
                           VkImageLayout imageLayout,
                           std::span<const VkBufferImageCopy> regions);
    void copyImageToBuffer(VkImage srcImage,
                           VkImageLayout imageLayout,
                           VkBuffer dstBuffer,
                           std::span<const VkBufferImageCopy> regions);
    void blitImage(VkImage srcImage,
                   VkImageLayout srcImageLayout,
                   VkImage dstImage,
                   VkImageLayout dstImageLayout,
                   std::span<const VkImageBlit> regions,
                   VkFilter filter = VK_FILTER_LINEAR);

    /// Push constants
    void pushConstants(VkPipelineLayout layout,
                       VkShaderStageFlags stageFlags,
                       uint32 offset,
                       uint32 size,
                       const void* values);

    /// Submit command buffer to queue
    void submit(VkQueue queue,
                std::span<const VkSemaphore> waitSemaphores      = {},
                std::span<const VkPipelineStageFlags> waitStages = {},
                std::span<const VkSemaphore> signalSemaphores    = {},
                VkFence fence                                    = VK_NULL_HANDLE);
    /// if queue is VK_NULL_HANDLE, it will use the current frame's graphics queue
    /// make sure if you set it to VK_NULL_HANDLE that RenderContext is valid (already created)
    void submitSync(VkQueue queue = VK_NULL_HANDLE);

    /// Get the underlying command buffer handle
    VkCommandBuffer commandBuffer() const noexcept { return m_commandBuffer; }

private:
    VkDevice m_device               = VK_NULL_HANDLE;
    VkCommandBuffer m_commandBuffer = VK_NULL_HANDLE;
    std::shared_ptr<VkCommandPool> m_pool; // Keeps the command pool alive
    bool m_isRecording = false;
};

} // namespace R3::vulkan
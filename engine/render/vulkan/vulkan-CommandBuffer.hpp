#pragma once

#include <functional>
#include <memory>
#include <span>
#include <vector>
#include <vulkan/vulkan_core.h>
#include "api/Class.hpp"
#include "api/Types.hpp"
#include "vulkan-Fwd.hpp"
#include "vulkan-Handle.hpp"

namespace R3::vulkan {

class CommandBuffer {
public:
    R3_CTOR_DEFAULT(CommandBuffer);

private:
    /// Private constructor - use allocate() instead
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
    void beginRenderPass(const VkRenderPassBeginInfo& beginInfo,
                         VkSubpassContents contents = VK_SUBPASS_CONTENTS_INLINE);
    void endRenderPass();
    void nextSubpass(VkSubpassContents contents = VK_SUBPASS_CONTENTS_INLINE);

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
    void transitionImageLayout(VkImage image,
                               VkImageLayout oldLayout,
                               VkImageLayout newLayout,
                               VkImageSubresourceRange subresourceRange,
                               VkPipelineStageFlags srcStage = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
                               VkPipelineStageFlags dstStage = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT);

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

    void addDeferredCallback(std::move_only_function<void()>&& callback);

    /// Submit command buffer to queue
    void submit(VkQueue queue,
                std::span<const VkSemaphore> waitSemaphores      = {},
                std::span<const VkPipelineStageFlags> waitStages = {},
                std::span<const VkSemaphore> signalSemaphores    = {},
                VkFence fence                                    = VK_NULL_HANDLE);
    void submitSync(VkQueue queue);

    /// Get the underlying command buffer handle
    VkCommandBuffer commandBuffer() const noexcept { return m_commandBuffer; }

private:
    VkDevice m_device;
    VkCommandBuffer m_commandBuffer;
    std::shared_ptr<VkCommandPool> m_pool;                            // Keeps the command pool alive
    std::vector<std::move_only_function<void()>> m_deferredCallbacks; // Called after submit
    bool m_isRecording = false;
};

} // namespace R3::vulkan
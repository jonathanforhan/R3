#pragma once

#include <memory>
#include <span>
#include <vector>
#include <vulkan/vulkan.h>
#include "engine/api/Api.hpp"
#include "engine/api/Class.hpp"
#include "engine/api/Types.hpp"
#include "engine/render/Buffer.hpp"
#include "engine/render/CommandBuffer.hpp"
#include "engine/render/Image.hpp"
#include "vulkan-Fwd.hpp"

namespace R3::vulkan {

class R3_API CommandBuffer : public ICommandBuffer {
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

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// State Commands
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void begin(VkCommandBufferUsageFlags flags = 0);
    void end();
    void reset(VkCommandBufferResetFlags flags = 0);
    void beginRendering(const VkRenderingInfo& beginInfo);
    void endRendering();

    virtual void beginCommands() override { begin(); }
    virtual void endCommands() override { end(); }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Bind Commands
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void bindGraphicsPipeline(VkPipeline pipeline);
    void bindComputePipeline(VkPipeline pipeline);
    void bindDescriptorSets(const VkBindDescriptorSetsInfo& bindDescriptorSetsInfo);
    void bindVertexBuffers(uint32 firstBinding,
                           std::span<const VkBuffer> buffers,
                           std::span<const VkDeviceSize> offsets);
    void bindIndexBuffer(VkBuffer buffer, VkDeviceSize offset, VkIndexType indexType);

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Graphics Pipeline Commands
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void draw(uint32 vertexCount, uint32 instanceCount = 1, uint32 firstVertex = 0, uint32 firstInstance = 0);
    void drawIndexed(uint32 indexCount,
                     uint32 instanceCount = 1,
                     uint32 firstIndex    = 0,
                     int32 vertexOffset   = 0,
                     uint32 firstInstance = 0);

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Compute Pipeline Commands
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void dispatch(uint32 groupCountX, uint32 groupCountY, uint32 groupCountZ);

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Dynamic State Commands
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void setViewport(const VkViewport& viewport);
    void setScissor(const VkRect2D& scissor);
    void setCullMode(VkCullModeFlags cullMode);
    void setLineWidth(float lineWidth);
    void setFrontFace(VkFrontFace frontFace);
    void setDepthTestEnable(bool enable);
    void pushConstants(const VkPushConstantsInfo& pushConstantsInfo);

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Transfer Commands
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void copyBuffer(const VkCopyBufferInfo2& copyInfo);
    void copyImage(const VkCopyImageInfo2& copyInfo);
    void copyBufferToImage(const VkCopyBufferToImageInfo2& copyInfo);
    void copyImageToBuffer(const VkCopyImageToBufferInfo2& copyInfo);
    void blitImage(const VkBlitImageInfo2& blitInfo);
    void resolveImage(const VkResolveImageInfo2& resolveInfo);

    virtual void copyBuffer(const Buffer& src, Buffer& dst) override;
    virtual void copyBuffer(const Buffer& src, usize srcOffset, Buffer& dst, usize dstOffset, usize size) override;
    virtual void copyImage(const Image& src, Image& dst) override;
    virtual void copyImage(const Image& src, usize3 srcOffset, Image& dst, usize3 dstOffset, usize3 extent) override;
    virtual void copyBufferToImage(const Buffer& src, Image& dst) override;
    virtual void copyBufferToImage(const Buffer& src,
                                   usize srcOffset,
                                   Image& dst,
                                   usize3 dstOffset,
                                   usize3 dstExtent) override;
    virtual void copyImageToBuffer(const Image& src, Buffer& dst) override;
    virtual void copyImageToBuffer(const Image& src,
                                   usize3 srcOffset,
                                   usize3 srcExtent,
                                   Buffer& dst,
                                   usize dstOffset) override;

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Synchronization Commands
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void pipelineBarrier(VkDependencyInfo dependencyInfo);

    /// Get the underlying command buffer handle
    VkCommandBuffer commandBuffer() const noexcept { return m_commandBuffer; }

private:
    VkDevice m_device               = VK_NULL_HANDLE;
    VkCommandBuffer m_commandBuffer = VK_NULL_HANDLE;
    std::shared_ptr<VkCommandPool> m_pool; // Keeps the command pool alive
    bool m_isRecording = false;
};

} // namespace R3::vulkan
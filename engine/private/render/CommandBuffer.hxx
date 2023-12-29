#pragma once

/// @file CommandBuffer.hxx
/// CommandBuffer is used to store commands, operations or recorded on them and them sumbitted to a queue

#include <span>
#include <vector>
#include "render/Fence.hxx"
#include "render/RenderApi.hxx"

namespace R3 {

/// @brief Flags for the usage of a command buffer
enum class CommandBufferUsage {
    Nil = 0,
    OneTimeSubmit,
    RenderPassContinue,
    SimultaneousUse,
};

/// @brief CommandBuffer Sumbit Specification
/// Accepts spans of the handles to the R3 Semaphores to not have to convert and dynamically allocate memory
struct CommandBufferSumbitSpecification {
    PipelineStage::Flags waitStages;                    ///< Wait on these Pipeline stages (bitwise | the flags)
    std::span<const NativeRenderObject> waitSemaphores; ///< Wait on these Semaphores
    std::span<NativeRenderObject> signalSemaphores;     ///< Trigger Semaphores when complete
    Ref<Fence> fence;                                   ///< (optional) Signal this Fence when complete
};

/// @brief CommandBuffer Present Specification
struct CommandBufferPresentSpecification {
    std::span<const NativeRenderObject> waitSemaphores; ///< Wait on these Semaphores
    uint32 currentImageIndex;                           ///< Index of current Image being drawn to
};

/// @brief Command Buffer Specification
struct CommandBufferSpecification {
    const LogicalDevice& logicalDevice; ///< LogicalDevice
    const Swapchain& swapchain;         ///< Swapchain
    const CommandPool& commandPool;     ///< Parent CommandPool
    uint32 commandBufferCount;          ///< Number of CommandBuffers to allocate
};

/// @brief CommandBuffer used to record operations
/// Command Buffers are allocated many at a time by a CommandPool
class CommandBuffer : public NativeRenderObject {
public:
    DEFAULT_CONSTRUCT(CommandBuffer);
    NO_COPY(CommandBuffer);
    DEFAULT_MOVE(CommandBuffer);

    /// @brief Allocate a vector of CommandBuffers
    /// @param spec
    /// @return vector<CommandBuffer>
    static std::vector<CommandBuffer> allocate(const CommandBufferSpecification& spec);

    /// @brief Free a group of CommandBuffers from Memory
    /// @param commandBuffers
    static void free(std::vector<CommandBuffer>& commandBuffers);

    /// @brief Free this specific CommandBuffer
    void free();

    /// @brief Reset a CommandBuffer to initial state
    void resetCommandBuffer() const;

    /// @brief Start recording a CommandBuffer with given flags
    /// @param usage Usage Flags
    void beginCommandBuffer(CommandBufferUsage usage = {}) const;

    /// @brief Stop recording commands
    void endCommandBuffer() const;

    /// @brief Begin a new RenderPass with this CommandBuffer
    /// @param renderPass
    /// @param framebuffer
    void beginRenderPass(const RenderPass& renderPass, const Framebuffer& framebuffer) const;

    /// @brief End the RenderPass
    void endRenderPass() const;

    /// @brief Bind a Pipeline to this CommandBuffer
    /// At binding the dynamic viewport and scissor will be set
    /// @param graphicsPipeline
    void bindPipeline(const GraphicsPipeline& graphicsPipeline) const;

    /// @brief Bind VertexBuffer to CommandBuffer
    /// @param vertexBuffer
    void bindVertexBuffer(const VertexBuffer& vertexBuffer) const;

    /// @brief Bind IndexBuffer<uint16> to CommandBuffer
    /// @param indexBuffer
    void bindIndexBuffer(const IndexBuffer<uint16>& indexBuffer) const;

    /// @brief Bind IndexBuffer<uint32> to CommandBuffer
    /// @param indexBuffer
    void bindIndexBuffer(const IndexBuffer<uint32>& indexBuffer) const;

    /// @brief Bind DescriptorSet to CommandBuffer, this tells the Buffer about shader binding data
    /// @param pipelineLayout
    /// @param descriptorSets
    void bindDescriptorSet(const PipelineLayout& pipelineLayout, const DescriptorSet& descriptorSets) const;

    /// @brief Submit to the CommandBuffer's LogicalDevice's Graphics Queue from given spec
    /// @param spec
    void submit(const CommandBufferSumbitSpecification& spec) const;

    /// @brief A convience function to easily do a blocking one-time submit on a CommandBuffer
    /// This is used mainly for Copies, Transitions, Blitting etc
    void oneTimeSubmit() const;

    /// @brief Present to CommandBuffer's LogicalDevice's Present Queue
    /// @note THIS FUNCTION MAY THROW. This function will throw if there is a presentation error
    /// Check the return value get error code
    /// @param spec
    /// @return Error code (per platform) vulkan -> VkResult
    int32 present(const CommandBufferPresentSpecification& spec) const;

private:
    Ref<const LogicalDevice> m_logicalDevice;
    Ref<const Swapchain> m_swapchain;
    Ref<const CommandPool> m_commandPool;
};

} // namespace R3
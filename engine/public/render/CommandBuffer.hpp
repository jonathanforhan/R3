#pragma once

#include <span>
#include <vector>
#include "api/Types.hpp"
#include "render/NativeRenderObject.hpp"
#include "render/RenderFwd.hpp"

namespace R3 {

enum class CommandBufferFlags {
    Nil = 0,
    OneTimeSubmit,
    RenderPassContinue,
    SimultaneousUse,
};

struct CommandBufferSpecification {
    Ref<const LogicalDevice> logicalDevice;
    Ref<const Swapchain> swapchain;
    Ref<const CommandPool> commandPool;
    uint32 commandBufferCount;
};

class CommandBuffer : public NativeRenderObject {
public:
    static std::vector<CommandBuffer> allocate(const CommandBufferSpecification& spec);
    static void free(std::vector<CommandBuffer>& commandBuffers);
    void free();

    void resetCommandBuffer() const;
    void beginCommandBuffer(CommandBufferFlags flags = {}) const;
    void endCommandBuffer() const;
    void beginRenderPass(const RenderPass& renderPass, const Framebuffer& framebuffer) const;
    void endRenderPass() const;
    void bindPipeline(const GraphicsPipeline& graphicsPipeline) const;
    void bindVertexBuffer(const VertexBuffer& vertexBuffer) const;
    void bindIndexBuffer(const IndexBuffer<uint16>& indexBuffer) const;
    void bindIndexBuffer(const IndexBuffer<uint32>& indexBuffer) const;
    void bindDescriptorSet(const PipelineLayout& pipelineLayout, const DescriptorSet& descriptorSets) const;
    void oneTimeSubmit() const;

private:
    CommandBufferSpecification m_spec;
};

} // namespace R3
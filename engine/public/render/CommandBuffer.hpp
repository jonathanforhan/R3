#pragma once

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
    const LogicalDevice* logicalDevice;
    const Swapchain* swapchain;
    const CommandPool* commandPool;
};

class CommandBuffer : public NativeRenderObject {
public:
    void create(const CommandBufferSpecification& spec);
    void destroy();

    void resetCommandBuffer() const;
    void beginCommandBuffer(CommandBufferFlags flags = {}) const;
    void endCommandBuffer() const;
    void beginRenderPass(const RenderPass& renderPass, const Framebuffer& framebuffer) const;
    void endRenderPass() const;
    void bindPipeline(const GraphicsPipeline& graphicsPipeline) const;
    void bindVertexBuffers(const std::vector<VertexBuffer>& vertexBuffers) const;
    void bindIndexBuffer(const IndexBuffer<uint16>& indexBuffer) const;
    void bindIndexBuffer(const IndexBuffer<uint32>& indexBuffer) const;

private:
    CommandBufferSpecification m_spec;
};

} // namespace R3
#pragma once

#include "api/Types.hpp"
#include "render/NativeRenderObject.hpp"
#include "render/RenderFwd.hpp"

namespace R3 {

struct CommandBufferSpecification {
    const LogicalDevice* logicalDevice;
    const Swapchain* swapchain;
    const CommandPool* commandPool;
};

class CommandBuffer : public NativeRenderObject {
public:
    void create(const CommandBufferSpecification& spec);
    void destroy();

    void resetCommandBuffer();
    void beginCommandBuffer();
    void endCommandBuffer();
    void beginRenderPass(const RenderPass& renderPass, const Framebuffer& framebuffer);
    void endRenderPass();
    void bindPipeline(const GraphicsPipeline& graphicsPipeline);

private:
    CommandBufferSpecification m_spec;
};

} // namespace R3
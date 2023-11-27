#pragma once

#include "render/NativeRenderObject.hpp"
#include "render/RenderFwd.hpp"
#include "render/CommandBuffer.hpp"

namespace R3 {

struct CommandPoolSpecification {
    const LogicalDevice* logicalDevice;
    const Swapchain* swapchain;
};

class CommandPool : public NativeRenderObject {
public:
    void create(const CommandPoolSpecification& spec);
    void destroy();

    CommandBuffer& commandBuffer() { return m_commandBuffer; }

private:
    CommandPoolSpecification m_spec;

    CommandBuffer m_commandBuffer;
};

} // namespace R3
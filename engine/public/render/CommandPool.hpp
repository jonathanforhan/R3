#pragma once

#include <vector>
#include "render/CommandBuffer.hpp"
#include "render/NativeRenderObject.hpp"
#include "render/RenderFwd.hpp"

namespace R3 {

enum class CommandPoolFlags {
    Transient,
    Reset,
    Protected,
};

struct CommandPoolSpecification {
    const LogicalDevice* logicalDevice;
    const Swapchain* swapchain;
    CommandPoolFlags flags;
    usize commandBufferCount;
};

class CommandPool : public NativeRenderObject {
public:
    void create(const CommandPoolSpecification& spec);
    void destroy();

    std::vector<CommandBuffer>& commandBuffers() { return m_commandBuffers; }
    const std::vector<CommandBuffer>& commandBuffers() const { return m_commandBuffers; }

private:
    CommandPoolSpecification m_spec;

    std::vector<CommandBuffer> m_commandBuffers;
};

} // namespace R3
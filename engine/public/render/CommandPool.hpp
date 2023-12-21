#pragma once

#include <span>
#include <vector>
#include "api/Types.hpp"
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
    Ref<const LogicalDevice> logicalDevice;
    Ref<const Swapchain> swapchain;
    CommandPoolFlags flags;
    uint32 commandBufferCount;
};

class CommandPool : public NativeRenderObject {
public:
    CommandPool() = default;
    CommandPool(const CommandPoolSpecification& spec);
    CommandPool(CommandPool&&) noexcept = default;
    CommandPool& operator=(CommandPool&&) noexcept = default;
    ~CommandPool();

    std::span<CommandBuffer> commandBuffers() { return m_commandBuffers; }
    std::span<const CommandBuffer> commandBuffers() const { return m_commandBuffers; }

private:
    Ref<const LogicalDevice> m_logicalDevice;
    std::vector<CommandBuffer> m_commandBuffers;
};

} // namespace R3
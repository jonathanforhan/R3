#pragma once

/// @brief Pool from which CommandBuffers are allocated
/// A thread can only write to 1 CommandPool at a time, so a thread has a CommandPool each

#include "render/CommandBuffer.hpp"
#include "render/RenderApi.hpp"

namespace R3 {

/// @brief Command Pool Type
enum class R3_API CommandPoolType {
    Transient, ///< Used Short-lived CommandPools
    Reset,     ///< Use and can be reset to initial state
    Protected, ///< Protected CommandBuffer
};

/// @brief Command Pool Specification
struct R3_API CommandPoolSpecification {
    const LogicalDevice& logicalDevice; ///< Logical Device
    const Swapchain& swapchain;         ///< Swapchain
    CommandPoolType type;               ///< CommandPool type
    uint32 commandBufferCount;          ///< CommandBuffer count
};

/// @brief CommandPool is used to allocate CommandBuffers from
/// The CommandPool owns the Buffers and the Buffers are accessed through the pool
/// The the CommandPool can not be accessed by multiple threads at a time
class R3_API CommandPool : public NativeRenderObject {
public:
    DEFAULT_CONSTRUCT(CommandPool);
    NO_COPY(CommandPool);
    DEFAULT_MOVE(CommandPool);

    /// @brief Construct CommandPool from spec
    /// @param spec
    CommandPool(const CommandPoolSpecification& spec);

    /// @brief Free Pool and Buffers
    ~CommandPool();

    /// @brief Query CommandBuffers
    /// @return CommandBuffers
    [[nodiscard]] constexpr std::span<CommandBuffer> commandBuffers() { return m_commandBuffers; }

    /// @brief Query CommandBuffers const
    /// @return CommandBuffers
    [[nodiscard]] constexpr std::span<const CommandBuffer> commandBuffers() const { return m_commandBuffers; }

private:
    Ref<const LogicalDevice> m_logicalDevice;
    std::vector<CommandBuffer> m_commandBuffers;
};

} // namespace R3
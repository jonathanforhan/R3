////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file vulkan-CommandPool.hxx
/// @copyright GNU Public License
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#if R3_VULKAN

#include "vulkan-fwd.hxx"
#include "vulkan-VulkanObject.hxx"
#include <api/Construct.hpp>
#include <api/Result.hpp>
#include <api/Types.hpp>
#include <vector>
#include <vulkan/vulkan.h>

namespace R3::vulkan {

/// @brief Vulkan CommandPool Specification.
struct CommandPoolSpecification {
    const LogicalDevice& device;                                                      ///< Valid LogicalDevice.
    const Queue& queue;                                                               ///< Valid Queue.
    VkCommandPoolCreateFlags flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT; ///< Flags for creatation.
    uint32 commandBufferCount; ///< Ammount of VkCommandBuffers to allocate.
};

/// @brief Vulkan CommandPool RAII wrapper.
/// Owns all CommandBuffers, allocated on create and destroys in destructor.
/// https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/VkCommandPool.html
class CommandPool : public VulkanObject<VkCommandPool> {
public:
    DEFAULT_CONSTRUCT(CommandPool);
    NO_COPY(CommandPool);
    DEFAULT_MOVE(CommandPool);

    /// @brief Create CommandPool from spec, allocating desired buffer count.
    /// @param spec
    /// @return CommandPool | InitializationFailure | AllocationFailure
    static Result<CommandPool> create(const CommandPoolSpecification& spec);

    /// @brief Destroy CommandPool and frees all CommandBuffers allocated.
    ~CommandPool();

    /// @brief Index into CommandPool to get a VkCommandBuffer at index i.
    /// @param i Index, MUST be less than the size of allocated CommandBuffers
    /// @return VkCommandBuffer at index i
    VkCommandBuffer operator[](int i) { return m_commandBuffers[i]; }

private:
    VkDevice m_device = VK_NULL_HANDLE;
    std::vector<VkCommandBuffer> m_commandBuffers;
};

} // namespace R3::vulkan

#endif // R3_VULKAN
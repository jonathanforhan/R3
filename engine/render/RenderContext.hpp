/**
 * @file RenderContext.hpp
 * @brief Vulkan rendering context management for the R3 engine
 */

#pragma once

#include <vulkan/vulkan_core.h>
#include "Flags.hpp"
#include "Handle.hpp"
#include "Types.hpp"

namespace R3 {

class Window;

struct Queue {
    QueueType type = QueueType::Graphics;
    uint32 index   = 0xFFFFFFFF;
#if R3_VULKAN
    VkQueue handle = nullptr;
#endif
};

class RenderContext {
public:
    RenderContext() = default;

    explicit RenderContext(Window& window);

    ~RenderContext() noexcept;

    RenderContext(const RenderContext&)            = delete;
    RenderContext& operator=(const RenderContext&) = delete;

    RenderContext(RenderContext&&) noexcept            = default;
    RenderContext& operator=(RenderContext&&) noexcept = default;

    void waitIdle();

    const Queue& graphicsQueue() const noexcept { return m_graphicsQueue; }

    const Queue& presentQueue() const noexcept { return m_presentQueue; }

    const Queue& computeQueue() const noexcept { return m_computeQueue; }

    uint32 deviceMemoryTypeIndex(uint32 typeFilter, MemoryProperties properties) const;

    VkDevice device() noexcept { return m_device; }

    VkPhysicalDevice physicalDevice() noexcept { return m_physicalDevice; }

    VkSurfaceKHR surface() noexcept { return m_surface; }

private:
    Queue m_graphicsQueue;
    Queue m_presentQueue;
    Queue m_computeQueue;

#if R3_VULKAN
    Handle<VkInstance> m_instance;
    Handle<VkDebugUtilsMessengerEXT> m_debug;
    Handle<VkSurfaceKHR> m_surface;
    Handle<VkPhysicalDevice> m_physicalDevice;
    Handle<VkDevice> m_device;
#endif
};

} // namespace R3

/**
 * @file RenderContext.hpp
 * @brief Vulkan rendering context management for the R3 engine
 */

#pragma once

#include <vulkan/vulkan_core.h>
#include "Types.hpp"

namespace R3 {

class Window;

enum class QueueType {
    Present,
    Graphics,
    Compute,
    Transfer,
};

struct Queue {
    QueueType type = QueueType::Graphics;
    VkQueue handle = VK_NULL_HANDLE;
    uint32 index   = static_cast<uint32>(-1);
};

class RenderContext {
public:
    void create(Window& window);

    void destroy() noexcept;

    void waitIdle();

    VkDevice device() noexcept { return m_logicalDevice; }

    VkPhysicalDevice physicalDevice() noexcept { return m_physicalDevice; }

    VkSurfaceKHR surface() noexcept { return m_surface; }

    const Queue& graphicsQueue() const noexcept { return m_graphicsQueue; }

    const Queue& presentQueue() const noexcept { return m_presentQueue; }

    const Queue& computeQueue() const noexcept { return m_computeQueue; }

private:
    VkInstance m_instance             = VK_NULL_HANDLE;
    VkDebugUtilsMessengerEXT m_debug  = VK_NULL_HANDLE;
    VkSurfaceKHR m_surface            = VK_NULL_HANDLE;
    VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;
    VkDevice m_logicalDevice          = VK_NULL_HANDLE;
    Queue m_graphicsQueue;
    Queue m_presentQueue;
    Queue m_computeQueue;
};

} // namespace R3

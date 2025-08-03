/**
 * @file RenderContext.hpp
 * @brief Vulkan rendering context management for the R3 engine
 */

#pragma once

#include <VkBootstrap.h>
#include <vulkan/vulkan_core.h>
#include "Types.hpp"

namespace R3 {

class Window;

struct Queue {
    vkb::QueueType type = vkb::QueueType::graphics;
    VkQueue handle      = VK_NULL_HANDLE;
    uint32 index        = uint32(-1);
};

class RenderContext {
public:
    void create(Window& window) noexcept(false);

    void destroy() noexcept(true);

    VkDevice device() noexcept(true) { return m_logicalDevice; }
    const VkDevice device() const noexcept(true) { return m_logicalDevice; }

    VkPhysicalDevice physicalDevice() noexcept(true) { return m_physicalDevice; }
    const VkPhysicalDevice physicalDevice() const noexcept(true) { return m_physicalDevice; }

    VkSurfaceKHR surface() noexcept(true) { return m_surface; }
    const VkSurfaceKHR surface() const noexcept(true) { return m_surface; }

    const Queue& graphicsQueue() const noexcept(true) { return m_graphicsQueue; }

    const Queue& presentQueue() const noexcept(true) { return m_presentQueue; }

    const Queue& computeQueue() const noexcept(true) { return m_computeQueue; }

private:
    vkb::Instance createInstance() const noexcept(false);

    VkSurfaceKHR createSurface(Window& window) const noexcept(false);

    vkb::PhysicalDevice selectPhysicalDevice(const vkb::Instance& instance) const noexcept(false);

    vkb::Device createLogicalDevice(const vkb::PhysicalDevice& physicalDevice) const noexcept(false);

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

#pragma once

#if R3_OPENGL
#include <glad/glad.h>
#elif R3_VULKAN
#include <vulkan/vulkan.h>
#include "vulkan/vulkan-ColorBuffer.hxx"
#include "vulkan/vulkan-DepthBuffer.hxx"
#include "vulkan/vulkan-Framebuffer.hxx"
#include "vulkan/vulkan-Instance.hxx"
#include "vulkan/vulkan-LogicalDevice.hxx"
#include "vulkan/vulkan-PhysicalDevice.hxx"
#include "vulkan/vulkan-RenderPass.hxx"
#include "vulkan/vulkan-Surface.hxx"
#include "vulkan/vulkan-Swapchain.hxx"
#endif

#include <vector>
#include "api/Construct.hpp"
#include "render/Window.hpp"

namespace R3 {

/**
 * @brief Singleton renderer for R3 Application, uses selected graphics API
 *
 * The user will never touch the Renderer, instead there will be magic classes
 * that the user must inherit from when creating their Entity and that will
 * be how an object is Rendered
 *
 * Private fields may contain API-Specfic code but public fields must not
 */
class Renderer {
public:
    NO_COPY(Renderer);
    NO_MOVE(Renderer);

    /**
     * @brief Creates Renderer, uses RAII for Vulkan Objects
     *
     * The Renderer is tied to a specific Window
     */
    explicit Renderer(Window& window);

    // TODO docs
    void render();

private:
    Window& m_window;

#if R3_OPENGL
    // ...
#elif R3_VULKAN
    // render
    vulkan::Instance m_instance;
    vulkan::Surface m_surface;
    vulkan::PhysicalDevice m_physicalDevice;
    vulkan::LogicalDevice m_device;
    vulkan::Swapchain m_swapchain;
    vulkan::RenderPass m_renderPass;
    vulkan::ColorBuffer m_colorBuffer;
    vulkan::DepthBuffer m_depthBuffer;
    std::vector<vulkan::Framebuffer> m_framebuffers;
#endif
};

} // namespace R3

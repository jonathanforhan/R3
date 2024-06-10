#if R3_VULKAN

#include "render/Renderer.hxx"
#include "render/Window.hpp"
#include "vulkan-ColorBuffer.hxx"
#include "vulkan-DepthBuffer.hxx"
#include "vulkan-Instance.hxx"
#include "vulkan-LogicalDevice.hxx"
#include "vulkan-PhysicalDevice.hxx"
#include "vulkan-RenderPass.hxx"
#include "vulkan-Surface.hxx"
#include "vulkan-Swapchain.hxx"
#include <vector>

namespace R3 {
using namespace ::R3::vulkan;

Renderer::Renderer(Window& window)
    : m_window(window) {
    // Instance Extensions
    std::vector<const char*> extensions(Instance::queryRequiredExtensions());
    std::vector<const char*> validationLayers;

    // Validation
#if R3_VALIDATION_LAYERS
    extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    validationLayers.push_back("VK_LAYER_KHRONOS_validation");
#endif

    // Instance
    m_instance = Instance({
        .applicationName  = "R3 Game Engine",
        .extensions       = extensions,
        .validationLayers = validationLayers,
    });

    // Surface
    m_surface = Surface({
        .instance = m_instance,
        .window   = m_window,
    });

    // Physical Device
    m_physicalDevice = PhysicalDevice({
        .instance   = m_instance,
        .surface    = m_surface,
        .extensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME},
    });

    // Logical Device and Queues
    m_device = {{
        .instance       = m_instance,
        .surface        = m_surface,
        .physicalDevice = m_physicalDevice,
    }};

    // Swapchain
    m_swapchain = Swapchain({
        .surface        = m_surface,
        .physicalDevice = m_physicalDevice,
        .device         = m_device,
        .window         = m_window,
    });

    // RenderPass
    m_renderPass = RenderPass({
        .physicalDevice = m_physicalDevice,
        .device         = m_device,
        .colorAttachment =
            {
                .format      = m_swapchain.surfaceFormat(),
                .sampleCount = m_physicalDevice.sampleCount(),
            },
        .depthAttachment =
            {
                .sampleCount = m_physicalDevice.sampleCount(),
            },
    });

    // ColorBuffer
    // DepthBuffer
    // Framebuffers
    // recreating the swapchain is an easy way to reset attachment buffers and framebuffers
    m_swapchain.recreate({
        .window         = m_window,
        .surface        = m_surface,
        .physicalDevice = m_physicalDevice,
        .device         = m_device,
        .framebuffers   = m_framebuffers,
        .colorBuffer    = m_colorBuffer,
        .depthBuffer    = m_depthBuffer,
        .renderPass     = m_renderPass,
    });

    // CommandPool and CommandBuffers

    // Synchronization
}

void Renderer::render() {
    if (m_window.shouldResize()) {
        m_swapchain.recreate({
            .window         = m_window,
            .surface        = m_surface,
            .physicalDevice = m_physicalDevice,
            .device         = m_device,
            .framebuffers   = m_framebuffers,
            .colorBuffer    = m_colorBuffer,
            .depthBuffer    = m_depthBuffer,
            .renderPass     = m_renderPass,
        });

        m_window.setShouldResize(false);
    }
}

} // namespace R3

#endif // R3_VULKAN

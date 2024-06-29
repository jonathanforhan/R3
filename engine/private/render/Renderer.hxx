#pragma once

#if R3_OPENGL
#include <glad/glad.h>
#elif R3_VULKAN
#include <vulkan/vulkan.h>
#include "vulkan/vulkan-ColorBuffer.hxx"
#include "vulkan/vulkan-CommandPool.hxx"
#include "vulkan/vulkan-DepthBuffer.hxx"
#include "vulkan/vulkan-Fence.hxx"
#include "vulkan/vulkan-Framebuffer.hxx"
#include "vulkan/vulkan-Instance.hxx"
#include "vulkan/vulkan-LogicalDevice.hxx"
#include "vulkan/vulkan-PhysicalDevice.hxx"
#include "vulkan/vulkan-RenderPass.hxx"
#include "vulkan/vulkan-Semaphore.hxx"
#include "vulkan/vulkan-Surface.hxx"
#include "vulkan/vulkan-Swapchain.hxx"
#endif

#include <api/Construct.hpp>
#include <vector>
#include "render/Window.hpp"

// TODO FIXME (for testing)
// {
#include "Mesh.hxx"
#include "ShaderObjects.hxx"
#include "vulkan/vulkan-DescriptorPool.hxx"
#include "vulkan/vulkan-GraphicsPipeline.hxx"
#include "vulkan/vulkan-ShaderModule.hxx"
// }

namespace R3 {

/**
 * Renderer for R3 Application, uses selected graphics API
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
     * Creates Renderer, uses RAII for Vulkan Objects
     * The Renderer is tied to a specific Window
     */
    explicit Renderer(Window& window);

    ~Renderer();

    // TODO docs
    void render();

    void resize();

public:
    static constexpr int MAX_FRAMES_IN_FLIGHT = 3;

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
    vulkan::CommandPool m_commandPool;

    // sync
    vulkan::Semaphore m_imageAvailable[MAX_FRAMES_IN_FLIGHT];
    vulkan::Semaphore m_renderFinished[MAX_FRAMES_IN_FLIGHT];
    vulkan::Fence m_inFlight[MAX_FRAMES_IN_FLIGHT];
    uint32 m_currentFrame = 0;

    // TODO FIXME
    vulkan::ShaderModule m_vertexShader;
    vulkan::ShaderModule m_fragmentShader;
    VkDescriptorSetLayout m_descriptorSetLayout;
    vulkan::GraphicsPipeline m_graphicsPipeline;
#endif
};

} // namespace R3

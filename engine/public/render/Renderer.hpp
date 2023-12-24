#pragma once

/// @file Renderer.hpp
/// Main R3 Renderer
/// - Create Intance
/// - Create Surface
/// - Find optimal PhysicalDevice
/// - Create LogicalDevice, which then queries and stores Queues
/// - Create Swapchain
/// - Create RenderPass
/// - Allocate DepthBuffer
/// - Build Framebuffers from Swapchain Images
/// - Create Render CommandPool and Local CommandPool
/// - Build Synchronization Resources

#include "render/ColorBuffer.hpp"
#include "render/CommandPool.hpp"
#include "render/DepthBuffer.hpp"
#include "render/Fence.hpp"
#include "render/Framebuffer.hpp"
#include "render/Instance.hpp"
#include "render/LogicalDevice.hpp"
#include "render/ModelLoader.hpp"
#include "render/PhysicalDevice.hpp"
#include "render/RenderApi.hpp"
#include "render/RenderPass.hpp"
#include "render/Semaphore.hpp"
#include "render/Surface.hpp"
#include "render/Swapchain.hpp"
#include "render/Window.hpp"

namespace R3 {

/// @brief Renderer Specification
struct RendererSpecification {
    Window& window; ///< Window
};

/// @brief Main R3 Renderer
/// Constructed and owned by the Engine
class Renderer {
public:
    NO_COPY(Renderer);
    NO_MOVE(Renderer);

    /// @brief Construct Renderer from spec
    /// @param spec
    Renderer(const RendererSpecification& spec);

    /// @brief Render a frame, this renders new UI frame, records commands and submits to present
    void render();

    /// @brief Resize Framebuffers through Swapchain Recreatation
    void resize();

    /// @brief Setter from Renderer View Matrix
    /// @param view
    void setView(const mat4& view) { m_viewProjection.view = view; }

    /// @brief Setter from Renderer Projection Matrix
    /// @param projection
    void setProjection(const mat4& projection) { m_viewProjection.projection = projection; }

    /// @brief Wait idle for synchronization
    void waitIdle() const;

    /// @brief Get ModelLoader
    /// @return loader
    ModelLoader& modelLoader() { return m_modelLoader; }

private:
    // prepares ui frame
    void newFrame();

private:
    Window& m_window;
    Instance m_instance;
    Surface m_surface;
    PhysicalDevice m_physicalDevice;
    LogicalDevice m_logicalDevice;
    Swapchain m_swapchain;
    RenderPass m_renderPass;
    std::vector<Framebuffer> m_framebuffers;
    CommandPool m_commandPoolLocal; // used for small command buffer operations like CPU -> GPU copy
    CommandPool m_commandPool;      // used for the render command buffers
    ColorBuffer m_colorBuffer;
    DepthBuffer m_depthBuffer;

    // mirrors shader push constant layout
    struct ViewProjection {
        alignas(16) mat4 view = mat4(1.0f);
        alignas(16) mat4 projection = mat4(1.0f);
    } m_viewProjection;

    Semaphore m_imageAvailable[MAX_FRAMES_IN_FLIGHT];
    Semaphore m_renderFinished[MAX_FRAMES_IN_FLIGHT];
    Fence m_inFlight[MAX_FRAMES_IN_FLIGHT];
    uint32 m_currentFrame = 0;

    ModelLoader m_modelLoader;

    friend class UserInterface;
};

} // namespace R3
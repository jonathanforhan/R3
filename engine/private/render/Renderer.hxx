#pragma once

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

#include "editor/Editor.hxx"
#include "render/ColorBuffer.hxx"
#include "render/CommandPool.hxx"
#include "render/DepthBuffer.hxx"
#include "render/Fence.hxx"
#include "render/Framebuffer.hxx"
#include "render/Instance.hxx"
#include "render/LogicalDevice.hxx"
#include "render/ModelLoader.hxx"
#include "render/PhysicalDevice.hxx"
#include "render/RenderApi.hxx"
#include "render/RenderPass.hxx"
#include "render/Semaphore.hxx"
#include "render/ShaderObjects.hxx"
#include "render/Surface.hxx"
#include "render/Swapchain.hxx"
#include "render/Window.hxx"

namespace R3 {

namespace ui {
class UserInterface;
}

/// @brief Renderer Specification
struct RendererSpecification {
    Window& window;
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

    /// @brief Render the Editor UI, this makes calls to the m_ui member
    /// @param dt deltaTime gotten from Engine for displaying framerate and such
    void renderEditorInterface(double dt);

    /// @brief Get ModelLoader
    /// @return loader
    [[nodiscard]] constexpr ModelLoader& modelLoader() { return m_modelLoader; }

private:
    void updateLighting();

private:
    //--- Render
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

    //--- Sync
    Semaphore m_imageAvailable[MAX_FRAMES_IN_FLIGHT];
    Semaphore m_renderFinished[MAX_FRAMES_IN_FLIGHT];
    Fence m_inFlight[MAX_FRAMES_IN_FLIGHT];
    uint32 m_currentFrame = 0;

    //--- Uniform / Push Constants
    ViewProjection m_viewProjection;
    std::vector<PointLightShaderObject> m_pointLights;

    //--- Utility
    ColorBuffer m_objectPickerColorBuffer;
    DepthBuffer m_objectPickerDepthBuffer;
    Framebuffer m_objectPickerFrameBuffer;

    editor::Editor m_editor;
    ModelLoader m_modelLoader; // ModelLoader needs to know certain info about renderer so it's a member
};

} // namespace R3
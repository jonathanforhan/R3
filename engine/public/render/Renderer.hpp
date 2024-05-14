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

#include "editor/Editor.hpp"
#include "render/ColorBuffer.hpp"
#include "render/CommandPool.hpp"
#include "render/DepthBuffer.hpp"
#include "render/Fence.hpp"
#include "render/Framebuffer.hpp"
#include "render/Instance.hpp"
#include "render/LogicalDevice.hpp"
#include "render/PhysicalDevice.hpp"
#include "render/RenderApi.hpp"
#include "render/RenderPass.hpp"
#include "render/Semaphore.hpp"
#include "render/ShaderObjects.hpp"
#include "render/StorageBuffer.hpp"
#include "render/Surface.hpp"
#include "render/Swapchain.hpp"
#include "render/Window.hpp"
#include "render/model/ModelLoader.hpp"

namespace R3 {

/// @brief Renderer Specification
struct R3_API RendererSpecification {
    Window& window;
};

/// @brief Main R3 Renderer
/// Constructed and owned by the Engine
class R3_API Renderer {
public:
    NO_COPY(Renderer);
    NO_MOVE(Renderer);

    /// @brief Construct Renderer from spec
    /// @param spec
    Renderer(const RendererSpecification& spec);

    /// @brief Do any initialize needed now that we have connected to client code and have an Active Scene
    void preLoop();

    /// @brief Render a frame, this renders new UI frame, records commands and submits to present
    void render();

    /// @brief Resize Framebuffers through Swapchain Recreatation
    void resize();

    /// @brief Recreate context on dll swap
    void recreate();

    /// @brief Setter from Renderer View Matrix
    /// @param view
    void setView(const mat4& view) { m_viewProjection.view = view; }

    /// @brief Setter from Renderer Projection Matrix
    /// @param projection
    void setProjection(const mat4& projection) { m_viewProjection.projection = projection; }

    /// @brief Setter for Renderer Cursor Position
    /// @param position
    void setCursorPosition(vec2 position) { m_cursorPosition = position; }

    [[nodiscard]] uuid32 getHoveredEntity() const;

    /// @brief Render the Editor UI, this makes calls to the m_ui member
    /// @param dt deltaTime gotten from Engine for displaying framerate and such
    void renderEditorInterface(double dt);

    /// @brief Get ModelLoader
    /// @return loader
    [[nodiscard]] constexpr ModelLoader& modelLoader() { return m_modelLoader; }

    /// @brief Wait idle for synchronization
    void waitIdle() const;

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
    vec2 m_cursorPosition = vec2(0); // normalized
    StorageBuffer m_storageBuffer;

    editor::Editor m_editor;
    ModelLoader m_modelLoader; // ModelLoader needs to know certain info about renderer so it's a member
};

} // namespace R3

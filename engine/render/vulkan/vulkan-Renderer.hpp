#pragma once

#include <vector>
#include "api/Class.hpp"
#include "api/Types.hpp"
#include "render/RenderContext.hpp"
#include "render/ShaderObjects.hpp"
#include "render/Window.hpp"
#include "vulkan-Buffer.hpp"
#include "vulkan-CommandBuffer.hpp"
#include "vulkan-GraphicsPipeline.hpp"
#include "vulkan-Image.hpp"
#include "vulkan-RenderContext.hpp"
#include "vulkan-Shader.hpp"
#include "vulkan-Swapchain.hpp"

namespace R3::vulkan {

class Renderer {
public:
    R3_COPY_DELETE(Renderer);
    R3_MOVE_DELETE(Renderer);

    Renderer(Window& window, RenderContext& ctx);

    ~Renderer() noexcept;

    void draw(double dt);

    // needed because using dynamic rendering
    void transitionAttachmentsForRender(CommandBuffer& cmd, uint32 imageIndex);

    // needed because using dynamic rendering
    void transitionAttachmentsForPresent(CommandBuffer& cmd, uint32 imageIndex);

    void handleWindowResize();

    IRenderContext* context() noexcept { return &m_ctx; }

private:
    Window& m_window;     // must out-live renderer
    RenderContext& m_ctx; // must out-live renderer
    Swapchain m_swapchain;
    Image m_colorImage;
    Image m_depthImage;
    Shader m_vertexShader;
    Shader m_fragmentShader;
    ViewProjection m_viewProj;
    std::vector<Buffer> m_ubos;
    GraphicsPipeline m_graphicsPipeline;
};

} // namespace R3::vulkan
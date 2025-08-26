#pragma once

#include <vector>
#include "api/Class.hpp"
#include "render/RenderContext.hpp"
#include "render/Window.hpp"
#include "vulkan-Buffer.hpp"
#include "vulkan-Framebuffer.hpp"
#include "vulkan-GraphicsPipeline.hpp"
#include "vulkan-Image.hpp"
#include "vulkan-RenderContext.hpp"
#include "vulkan-RenderPass.hpp"
#include "vulkan-Shader.hpp"
#include "vulkan-Swapchain.hpp"

namespace R3::vulkan {

struct UniformBufferObject {
    alignas(16) fmat4 model;
    alignas(16) fmat4 view;
    alignas(16) fmat4 proj;
};

class Renderer {
public:
    R3_COPY_DELETE(Renderer);
    R3_MOVE_DELETE(Renderer);

    Renderer(Window& window, RenderContext& ctx);

    ~Renderer() noexcept;

    void render(double dt);

    void handleWindowResize();

    IRenderContext* context() noexcept { return &m_ctx; }

private:
    Window& m_window;     // must out-live renderer
    RenderContext& m_ctx; // must out-live renderer
    Swapchain m_swapchain;
    Image m_colorImage;
    Image m_depthImage;
    RenderPass m_renderPass;
    Shader m_vertexShader;
    Shader m_fragmentShader;
    UniformBufferObject m_ubo;
    std::vector<Buffer> m_ubos;
    GraphicsPipeline m_graphicsPipeline;
    std::vector<Framebuffer> m_framebuffers;
};

} // namespace R3::vulkan
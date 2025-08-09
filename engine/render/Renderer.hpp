#pragma once

#include <vector>
#include <vulkan/vulkan_core.h>
#include "Buffer.hpp"
#include "CommandAllocator.hpp"
#include "FrameSync.hpp"
#include "Framebuffer.hpp"
#include "GraphicsPipeline.hpp"
#include "RenderContext.hpp"
#include "RenderPass.hpp"
#include "Shader.hpp"
#include "Swapchain.hpp"

namespace R3 {

class Window;

class Renderer {
public:
    explicit Renderer(Window& window);

    Renderer(const Renderer&)            = delete;
    Renderer& operator=(const Renderer&) = delete;

    Renderer(Renderer&&) noexcept            = delete;
    Renderer& operator=(Renderer&&) noexcept = delete;

    ~Renderer() noexcept;

    void render();

private:
    Window& m_window; // must out-live renderer
    RenderContext m_ctx;
    Swapchain m_swapchain;
    RenderPass m_renderPass;
    GraphicsPipeline m_graphicsPipeline;
    CommandAllocator m_commandAllocator;
    Shader m_vertexShader;
    Shader m_fragmentShader;
    Buffer m_vertexBuffer;
    std::vector<Framebuffer> m_framebuffers;
    std::vector<VkCommandBuffer> m_commandBuffers;
    FrameSync m_frameSync;
};

} // namespace R3
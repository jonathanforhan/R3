#pragma once

#include <vector>
#include <vulkan/vulkan_core.h>
#include "Buffer.hpp"
#include "Camera.hpp"
#include "CommandAllocator.hpp"
#include "DescriptorAllocator.hpp"
#include "FrameSync.hpp"
#include "Framebuffer.hpp"
#include "GraphicsPipeline.hpp"
#include "RenderContext.hpp"
#include "RenderPass.hpp"
#include "Shader.hpp"
#include "Swapchain.hpp"

namespace R3 {

struct UniformBufferObject {
    alignas(16) mat4 model;
    alignas(16) mat4 view;
    alignas(16) mat4 proj;
};

class Window;

class Renderer {
public:
    explicit Renderer(Window& window);

    Renderer(const Renderer&)            = delete;
    Renderer& operator=(const Renderer&) = delete;

    Renderer(Renderer&&) noexcept            = delete;
    Renderer& operator=(Renderer&&) noexcept = delete;

    ~Renderer() noexcept;

    void render(double dt);

    void handleWindowResize();

private:
    Window& m_window; // must out-live renderer
    RenderContext m_ctx;
    Swapchain m_swapchain;
    RenderPass m_renderPass;
    DescriptorAllocator m_descriptorAllocator;
    std::vector<VkDescriptorSet> m_descriptorSets;
    GraphicsPipeline m_graphicsPipeline;
    CommandAllocator m_commandAllocator;
    Shader m_vertexShader;
    Shader m_fragmentShader;
    Buffer m_vertexBuffer;
    UniformBufferObject m_ubo;
    std::vector<Buffer> m_ubos;
    std::vector<Framebuffer> m_framebuffers;
    std::vector<VkCommandBuffer> m_commandBuffers;
    FrameSync m_frameSync;

    Camera m_camera;
};

} // namespace R3
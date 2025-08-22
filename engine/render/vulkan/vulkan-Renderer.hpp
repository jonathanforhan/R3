#pragma once

#include <vector>
#include <vulkan/vulkan_core.h>
#include "Camera.hpp"
#include "render/Window.hpp"
#include "vulkan-Buffer.hpp"
#include "vulkan-CommandAllocator.hpp"
#include "vulkan-DescriptorAllocator.hpp"
#include "vulkan-FrameSync.hpp"
#include "vulkan-Framebuffer.hpp"
#include "vulkan-GraphicsPipeline.hpp"
#include "vulkan-RenderContext.hpp"
#include "vulkan-RenderPass.hpp"
#include "vulkan-Shader.hpp"
#include "vulkan-Swapchain.hpp"
#include "vulkan-Texture.hpp"

namespace R3::vulkan {

struct UniformBufferObject {
    alignas(16) fmat4 model;
    alignas(16) fmat4 view;
    alignas(16) fmat4 proj;
};

class Renderer {
public:
    explicit Renderer(Window& window);

    ~Renderer() noexcept;

    Renderer(const Renderer&)            = delete;
    Renderer& operator=(const Renderer&) = delete;

    Renderer(Renderer&&) noexcept            = delete;
    Renderer& operator=(Renderer&&) noexcept = delete;

    void render(double dt);

    void handleWindowResize();

private:
    Window& m_window; // must out-live renderer
    RenderContext m_ctx;
    Swapchain m_swapchain;
    RenderPass m_renderPass;
    std::vector<Framebuffer> m_framebuffers;
    CommandAllocator m_commandAllocator;
    std::vector<VkCommandBuffer> m_commandBuffers;
    Shader m_vertexShader;
    Shader m_fragmentShader;
    Buffer m_vertexBuffer;
    Buffer m_indexBuffer;
    Texture m_texture;
    UniformBufferObject m_ubo;
    std::vector<Buffer> m_ubos;
    DescriptorAllocator m_descriptorAllocator;
    GraphicsPipeline m_graphicsPipeline;
    std::vector<VkDescriptorSet> m_descriptorSets;
    FrameSync m_frameSync;

    Camera m_camera;
};

} // namespace R3::vulkan
#pragma once

#include <vector>
#include <vulkan/vulkan_core.h>
#include "api/Class.hpp"
#include "render/Window.hpp"
#include "vulkan-Buffer.hpp"
#include "vulkan-CommandBuffer.hpp"
#include "vulkan-DescriptorAllocator.hpp"
#include "vulkan-FrameSync.hpp"
#include "vulkan-Framebuffer.hpp"
#include "vulkan-GraphicsPipeline.hpp"
#include "vulkan-Image.hpp"
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
    R3_COPY_DELETE(Renderer);
    R3_MOVE_DELETE(Renderer);

    explicit Renderer(Window& window);

    ~Renderer() noexcept;

    void render(double dt);

    void handleWindowResize();

private:
    Window& m_window; // must out-live renderer
    RenderContext m_ctx;
    Swapchain m_swapchain;
    Image m_colorImage;
    Image m_depthImage;
    RenderPass m_renderPass;
    std::vector<CommandBuffer> m_graphicsQueueCmds;
    std::vector<CommandBuffer> m_computeQueueCmds;
    Shader m_vertexShader;
    Shader m_fragmentShader;
    Buffer m_vertexBuffer;
    Buffer m_indexBuffer;
    Texture m_texture;
    UniformBufferObject m_ubo;
    std::vector<Buffer> m_ubos;
    DescriptorAllocator m_descriptorAllocator;
    std::vector<VkDescriptorSet> m_descriptorSets;
    GraphicsPipeline m_graphicsPipeline;
    std::vector<Framebuffer> m_framebuffers;
    FrameSync m_frameSync;
};

} // namespace R3::vulkan
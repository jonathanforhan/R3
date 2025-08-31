#pragma once

#include <vector>
#include "engine/api/Api.hpp"
#include "engine/api/Class.hpp"
#include "engine/api/Types.hpp"
#include "engine/render/RenderContext.hpp"
#include "engine/render/ShaderObjects.hpp"
#include "engine/render/Window.hpp"
#include "vulkan-Buffer.hpp"
#include "vulkan-CommandBuffer.hpp"
#include "vulkan-GraphicsPipeline.hpp"
#include "vulkan-Image.hpp"
#include "vulkan-RenderContext.hpp"
#include "vulkan-Shader.hpp"
#include "vulkan-Swapchain.hpp"
#include "vulkan-Texture.hpp"

namespace R3::vulkan {

class R3_API Renderer {
public:
    R3_COPY_DELETE(Renderer);
    R3_MOVE_DELETE(Renderer);

    Renderer(Window& window, RenderContext& ctx);

    ~Renderer() noexcept;

    void draw();

    // needed because using dynamic rendering
    void transitionAttachmentsForRender(CommandBuffer& cmd, uint32 imageIndex);

    // needed because using dynamic rendering
    void transitionAttachmentsForPresent(CommandBuffer& cmd, uint32 imageIndex);

    void handleWindowResize();

    IRenderContext* context() noexcept { return &m_ctx; }

private:
    void addDescriptorMemoryBarrier(CommandBuffer& cmd);

    void beginRenderingHelper(CommandBuffer& cmd, uint32 imageIndex);

    void bindPipelineHelper(CommandBuffer& cmd, const GraphicsPipeline& pipeline);

    void writeDescriptorSetsHelper(uint32 frameIndex, uint32 numLights);

    void submitHelper(CommandBuffer& cmd, uint32 frameIndex);

    void presentFrameHelper(uint32 frameIndex, uint32 imageIndex);

    uint32 updateLights(uint32 frameIndex);

private:
    Window& m_window;     // must out-live renderer
    RenderContext& m_ctx; // must out-live renderer
    Swapchain m_swapchain;
    Image m_colorImage;
    Image m_depthImage;
    ViewProjection m_viewProj;
    Shader m_vertexShader;
    Shader m_fragmentShader;
    Shader m_cubemapVertexShader;
    Shader m_cubemapFragmentShader;
    GraphicsPipeline m_graphicsPipeline;
    GraphicsPipeline m_cubemapPipeline;
    Texture m_cubemapTexture;
    uint32 m_cubemapTextureBinding = 0;
    std::vector<Buffer> m_ubos;
    std::vector<Buffer> m_lights;
};

} // namespace R3::vulkan
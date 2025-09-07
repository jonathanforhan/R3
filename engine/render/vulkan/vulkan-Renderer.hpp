#pragma once

#include <array>
#include <vector>
#include "engine/api/Api.hpp"
#include "engine/api/Class.hpp"
#include "engine/api/Types.hpp"
#include "engine/render/Buffer.hpp"
#include "engine/render/Image.hpp"
#include "engine/render/RenderContext.hpp"
#include "engine/render/ShaderObjects.hpp"
#include "engine/render/Texture.hpp"
#include "engine/render/Window.hpp"
#include "vulkan-CommandBuffer.hpp"
#include "vulkan-GraphicsPipeline.hpp"
#include "vulkan-RenderContext.hpp"
#include "vulkan-Shader.hpp"
#include "vulkan-Swapchain.hpp"

namespace R3::vulkan {

class R3_API Renderer {
public:
    R3_COPY_DELETE(Renderer);
    R3_MOVE_DELETE(Renderer);

    Renderer(Window& window, RenderContext& ctx);

    ~Renderer() noexcept;

    void draw();

    IRenderContext* context() noexcept { return &m_ctx; }

private:
    void shadowPass(CommandBuffer& cmd, uint32 frameIndex);

    void cubemapPass(CommandBuffer& cmd, uint32 frameIndex);

    // needed because using dynamic rendering
    void transitionAttachmentsForRender(CommandBuffer& cmd, uint32 imageIndex);

    // needed because using dynamic rendering
    void transitionAttachmentsForPresent(CommandBuffer& cmd, uint32 imageIndex);

    void handleWindowResize();

    void beginRenderingHelper(CommandBuffer& cmd, uint32 imageIndex);

    void bindPipelineHelper(CommandBuffer& cmd, const GraphicsPipeline& pipeline);

    void writeDescriptorSetsHelper(uint32 frameIndex, uint32 numLights);

    uint32 updateLights(uint32 frameIndex);

private:
    Window& m_window;     // must out-live renderer
    RenderContext& m_ctx; // must out-live renderer
    Swapchain m_swapchain;
    Image m_colorImage;
    Image m_depthImage;
    VertexUniformBufferObject m_ubo;
    Shader m_vertexShader;
    Shader m_fragmentShader;
    Shader m_cubemapVertexShader;
    Shader m_cubemapFragmentShader;
    Shader m_directionalShadowMapVertexShader;
    Shader m_directionalShadowMapFragmentShader;
    GraphicsPipeline m_graphicsPipeline;
    GraphicsPipeline m_cubemapPipeline;
    GraphicsPipeline m_directionalShadowMapPipeline;
    Texture m_cubemapTexture;
    uint32 m_cubemapTextureBinding = 0;
    Texture m_directionalShadowMapTexture;
    std::vector<R3::Buffer> m_ubos;
    std::vector<R3::Buffer> m_lights;
};

} // namespace R3::vulkan
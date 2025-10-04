#pragma once

#include <cstdint>
#include <vector>
#include "engine/api/Api.hpp"
#include "engine/api/Class.hpp"
#include "engine/api/Types.hpp"
#include "engine/render/Buffer.hpp"
#include "engine/render/Image.hpp"
#include "engine/render/RenderContext.hpp"
#include "engine/render/Shader.hpp"
#include "engine/render/ShaderObjects.hpp"
#include "engine/render/Texture.hpp"
#include "engine/render/Window.hpp"
#include "passes/vulkan-EditorPass.hpp"
#include "passes/vulkan-MainPass.hpp"
#include "passes/vulkan-ShadowPass.hpp"
#include "vulkan-CommandBuffer.hpp"
#include "vulkan-GraphicsPipeline.hpp"
#include "vulkan-RenderContext.hpp"
#include "vulkan-Swapchain.hpp"

namespace R3::vulkan {

class R3_API Renderer {
public:
    R3_COPY_DELETE(Renderer);
    R3_MOVE_DELETE(Renderer);

    Renderer(Window& window, RenderContext& ctx);

    ~Renderer() noexcept;

    void acquire();

    void update();

    void render();

    void present();

private:
    void buildPipelines();

    void setupShadowPass();
    void setupMainPasses();
    void setupEditorPasses();

    void handleMouseClick(CommandBuffer& cmd, uint32 imageIndex);

    // needed because using dynamic rendering
    void transitionAttachmentsForPresent(CommandBuffer& cmd, uint32 imageIndex);

    void handleWindowResize();

    void writeDescriptorSetsHelper(uint32 frameIndex, uint32 numLights);

    uint32 updateLights(uint32 frameIndex);

private:
    Window& m_window;     // must out-live renderer
    RenderContext& m_ctx; // must out-live renderer
    Swapchain m_swapchain;

    uint32 m_imageIndex   = UINT32_MAX;
    uint32 m_currentFrame = 0;

    std::vector<Image> m_colorImages;
    std::vector<Image> m_depthImages;
    std::vector<Image> m_idImages;

    PBRVertexUniformBufferObject m_ubo;
    Shader m_vertexShader;
    Shader m_fragmentShader;
    Shader m_cubemapVertexShader;
    Shader m_cubemapFragmentShader;
    Shader m_directionalShadowMapVertexShader;
    Shader m_directionalShadowMapFragmentShader;
    Shader m_editorVertexShader;
    Shader m_editorFragmentShader;
    GraphicsPipeline m_directionalShadowMapPipeline;
    GraphicsPipeline m_cubemapPipeline;
    GraphicsPipeline m_graphicsPipeline;
    GraphicsPipeline m_editorPipeline;
    Texture m_cubemapTexture;
    uint32 m_cubemapTextureBinding = 0;
    Texture m_directionalShadowMapTexture;
    std::vector<Buffer> m_ubos;
    std::vector<Buffer> m_lights;

    ShadowPass m_shadowPass;
    std::vector<MainPass> m_mainPasses;
    std::vector<EditorPass> m_editorPasses;

    std::vector<Buffer> m_idReadbackBuffers;
    uint32 m_selectedEntityID = 0xFFFF'FFFF;

    bool m_shouldReloadShaders = false;
};

} // namespace R3::vulkan
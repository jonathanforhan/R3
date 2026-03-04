#include "vulkan-Renderer.hpp"

#include <array>
#include <filesystem>
#include <format>
#include <iterator>
#include <span>
#include <vector>
#include <vulkan/vulkan.h>
#include <entt/entity/entity.hpp>
#include <entt/entity/registry.hpp>
#include <entt/entity/view.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "api/Exception.hpp"
#include "api/Types.hpp"
#include "components/LightComponent.hpp"
#include "core/Camera.hpp"
#include "core/Engine.hpp"
#include "core/Entity.hpp"
#include "core/EventHandler.hpp"
#include "core/ResourceManager.hpp"
#include "core/World.hpp"
#include "input/InputCodes.hpp"
#include "input/InputEvents.hpp"
#include "passes/vulkan-MainPass.hpp"
#include "passes/vulkan-ShadowPass.hpp"
#include "render/Buffer.hpp"
#include "render/Flags.hpp"
#include "render/Image.hpp"
#include "render/Shader.hpp"
#include "render/ShaderObjects.hpp"
#include "render/Texture.hpp"
#include "render/Window.hpp"
#include "vulkan-CommandBuffer.hpp"
#include "vulkan-DescriptorSet.hpp"
#include "vulkan-GraphicsPipeline.hpp"
#include "vulkan-RenderContext.hpp"
#include "vulkan-RenderPass.hpp"
#include "vulkan-Swapchain.hpp"

namespace R3::vulkan {

static glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 1.0f, 10.0f);
static glm::mat4 lightView =
    glm::lookAt(glm::vec3(-2.0f, 4.0f, -4.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
static glm::mat4 lightSpaceMatrix = lightProjection * lightView;

Renderer::Renderer(Window& window, RenderContext& ctx)
    : m_window(window),
      m_ctx{ctx} {
    //--- Swapchain
    //    - images
    //    - image views
    m_swapchain            = Swapchain{m_ctx, m_window.framebufferSize()};
    const uint32 maxFrames = m_ctx.maxFramesInFlight();

    //--- Color/Depth Image
    const uint32 msaaSamples = m_ctx.queryMaxUsableSampleCount();

    const usize3 extent      = {m_swapchain.extent().width, m_swapchain.extent().height, 1};
    const Format colorFormat = Format(m_ctx.swapchainFormat());
    const Format depthFormat = Format(m_ctx.queryDepthFormat());
    const Format idFormat    = Format::R32_UINT;

    for (uint32 i = 0; i < maxFrames; i++) {
        // main pass
        m_colorImages.emplace_back(extent, 1, msaaSamples, ImageUsage::ColorAttachment, colorFormat);
        m_depthImagesMSAA.emplace_back(extent, 1, msaaSamples, ImageUsage::DepthStencilAttachment, depthFormat);
        // editor pass
        m_idImages.emplace_back(extent, 1, 1, ImageUsage::ColorAttachment | ImageUsage::TransferSrc, idFormat);
        m_depthImages.emplace_back(extent, 1, 1, ImageUsage::DepthStencilAttachment, depthFormat);
    }

    buildPipelines();

    //--- Cubemap
    CommandBuffer& cmd = m_ctx.graphicsCommandBuffer(0);
    cmd.reset();
    cmd.begin();
    std::array<std::filesystem::path, 6> facePaths = {
        "assets/textures/skybox/right.jpg",
        "assets/textures/skybox/left.jpg",
        "assets/textures/skybox/top.jpg",
        "assets/textures/skybox/bottom.jpg",
        "assets/textures/skybox/front.jpg",
        "assets/textures/skybox/back.jpg",
    };
    {
        m_cubemapTexture        = Texture{m_ctx.graphicsCommandBuffer(0), facePaths, TextureType::Cubemap};
        m_cubemapTextureBinding = GResourceManager()->bindTexture("skybox", m_cubemapTexture);
    }
    cmd.end();
    m_ctx.submitSync(m_ctx.graphicsQueue(), cmd.commandBuffer());

    //--- Uniform Buffers
    float aspect = static_cast<float>(m_swapchain.extent().width) / static_cast<float>(m_swapchain.extent().height);

    m_ubo = {
        .view                = glm::lookAt(fvec3(2.0f, 2.0f, 2.0f), fvec3(0.0f, 0.0f, 0.0f), fvec3(0.0f, 0.0f, 1.0f)),
        .projection          = glm::perspective(glm::radians(45.0f), aspect, 0.1f, 25.0f),
        .lightViewProjection = lightSpaceMatrix,
    };

    m_ubos.resize(m_ctx.maxFramesInFlight());
    for (auto& ubo : m_ubos) {
        ubo = Buffer{sizeof(PBRVertexUniformBufferObject), BufferUsage::HostUniform};
    }

    //--- Storage Buffers
    m_lights.resize(m_ctx.maxFramesInFlight());
    for (auto& light : m_lights) {
        light = Buffer{sizeof(PointLightShaderObject) * 256, BufferUsage::HostStorage};
    }

    //--- Shadow Maps
    cmd.begin();
    {
        m_directionalShadowMapTexture = Texture{
            m_ctx.graphicsCommandBuffer(0),
            nullptr,
            1024,
            1024,
            1,
            TextureType::Depth,
        };
    }
    cmd.end();
    m_ctx.submitSync(m_ctx.graphicsQueue(), cmd.commandBuffer());

    setupShadowPass();
    setupMainPasses();
    setupEditorPasses();

    for (uint32 i = 0; i < maxFrames; i++) {
        m_idReadbackBuffers.emplace_back(sizeof(uint32), BufferUsage::HostReadback);
    }

    GWorld()->camera().setActive(true);

    GEventHandler()->bindEventListener(event::KeyPress, [this](const KeyboardEvent& e) noexcept {
        if (e.key == Key::R && (e.modifiers & InputModifierFlags::Control)) {
            m_shouldReloadShaders = true;
        }
    });
}

Renderer::~Renderer() noexcept {
    m_ctx.waitIdle();
}

void Renderer::acquire() {
    if (m_shouldReloadShaders) {
        m_ctx.waitIdle();

        try {
            buildPipelines();

            setupShadowPass();
            setupMainPasses();
            setupEditorPasses();

            m_shouldReloadShaders = false;
        } catch (...) {
            m_shouldReloadShaders = true;
        }
    }

    if (m_window.shouldResize()) {
        handleWindowResize();
        m_window.setShouldResize(false);
    }

    m_currentFrame = m_ctx.currentFrameIndex();
    m_ctx.waitForFrame(m_currentFrame);

    VkResult result = m_swapchain.acquireNextImage(m_ctx.imageAvailableSemaphore(m_currentFrame), m_imageIndex);
    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        handleWindowResize();
        m_window.setShouldResize(false);
    } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        throw Exception{std::format("Failed to acquire swap chain image: {}", static_cast<int>(result))};
    }
}

void Renderer::update() {
    CommandBuffer& cmd = m_ctx.graphicsCommandBuffer(m_currentFrame);

    // update view projection matrices in ubo
    m_ubo.view       = GWorld()->camera().view();
    m_ubo.projection = GWorld()->camera().projection();

    m_ubo.lightViewProjection = lightSpaceMatrix;
    m_ubos[m_currentFrame].copy(&m_ubo, 0, sizeof(m_ubo));

    // update lights
    uint32 numLights = updateLights(m_currentFrame);
    fvec3 lightPos;
    GWorld()->registry().view<LightComponent>().each([&](const LightComponent& light) { lightPos = light.position; });

    // readback selected entity ID if pending and the readback corresponds to the current image
    if (m_pendingReadback) {
        Entity hoveredEntityID = static_cast<Entity>(*((uint32*)m_idReadbackBuffers[m_imageIndex].data()));
        if (hoveredEntityID != m_hoveredEntityID) {
            m_hoveredEntityID = hoveredEntityID;
            GEventHandler()->emplace<HoveredEntityEvent>(event::HoveredEntity, m_hoveredEntityID);
        }
        m_pendingReadback = false;
    }

    writeDescriptorSetsHelper(m_currentFrame, numLights);

    cmd.reset();
    cmd.begin();

    VkDescriptorSet descriptorSet = m_ctx.descriptorSet(m_currentFrame).descriptorSet();

    // shadow pass
    m_shadowPass.setDescriptorSet(descriptorSet);
    m_shadowPass.setLightSpaceMatrix(lightSpaceMatrix);

    // main pass
    m_mainPasses[m_imageIndex].setDescriptorSet(descriptorSet);
    m_mainPasses[m_imageIndex].setLightCount(numLights);

    const ivec2 cursorPosition = static_cast<ivec2>(GWindow()->cursorPosition());
    handleMouseHover(cmd, m_imageIndex, cursorPosition.x, cursorPosition.y);

#if R3_EDITOR
    // editor pass
    m_editorPasses[m_imageIndex].setDescriptorSet(descriptorSet);
#endif
}

void Renderer::render() {
    CommandBuffer& cmd = m_ctx.graphicsCommandBuffer(m_currentFrame);
    m_shadowPass.execute(cmd);
    m_mainPasses[m_imageIndex].execute(cmd);
    m_editorPasses[m_imageIndex].execute(cmd);
    transitionAttachmentsForPresent(cmd, m_imageIndex);
    cmd.end();

    // Submit command buffer - use per-frame acquire, per-image render finished
    m_ctx.submit(m_ctx.graphicsQueue(),
                 cmd.commandBuffer(),
                 VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                 m_ctx.imageAvailableSemaphore(m_currentFrame),
                 m_ctx.renderFinishedSemaphore(m_currentFrame),
                 m_ctx.inFlightFence(m_currentFrame));
}

void Renderer::present() {
    // Present - use per-image semaphore
    const VkSemaphore signalSemaphores[] = {m_ctx.renderFinishedSemaphore(m_currentFrame)};
    const VkSwapchainKHR swapchains[]    = {m_swapchain.swapchain()};
    const VkPresentInfoKHR presentInfo{
        .sType              = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
        .waitSemaphoreCount = 1,
        .pWaitSemaphores    = signalSemaphores,
        .swapchainCount     = 1,
        .pSwapchains        = swapchains,
        .pImageIndices      = &m_imageIndex,
        .pResults           = nullptr,
    };
    VkResult result = vkQueuePresentKHR(m_ctx.graphicsQueue(), &presentInfo);
    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
        m_window.setShouldResize(true);
    } else if (result != VK_SUCCESS) {
        throw Exception{std::format("vkQueuePresentKHR returned: {}", static_cast<int>(result))};
    }

    m_ctx.advanceFrame();
}

void Renderer::buildPipelines() {
    //--- Shaders
    m_vertexShader   = Shader{"shaders/pbr.vert", ShaderStage::Vertex};
    m_fragmentShader = Shader{"shaders/pbr.frag", ShaderStage::Fragment};

    m_cubemapVertexShader   = Shader{"shaders/cubemap.vert", ShaderStage::Vertex};
    m_cubemapFragmentShader = Shader{"shaders/cubemap.frag", ShaderStage::Fragment};

    m_directionalShadowMapVertexShader   = Shader{"shaders/directional_shadow_map.vert", ShaderStage::Vertex};
    m_directionalShadowMapFragmentShader = Shader{"shaders/directional_shadow_map.frag", ShaderStage::Fragment};

    m_editorVertexShader   = Shader{"shaders/editor.vert", ShaderStage::Vertex};
    m_editorFragmentShader = Shader{"shaders/editor.frag", ShaderStage::Fragment};

    //--- Graphics Pipeline
    const VkDescriptorSetLayout layout = m_ctx.descriptorLayout();
    const VkFormat colorFormat         = m_swapchain.format();
    const uint32 msaaSamples           = m_ctx.queryMaxUsableSampleCount();
    const Format idFormat              = Format::R32_UINT;

    m_directionalShadowMapPipeline = GraphicsPipeline{
        m_ctx,
        m_directionalShadowMapVertexShader,
        m_directionalShadowMapFragmentShader,
        VK_SAMPLE_COUNT_1_BIT,
        {},
        {},
        {layout},
    };

    m_cubemapPipeline = GraphicsPipeline{
        m_ctx,
        m_cubemapVertexShader,
        m_cubemapFragmentShader,
        msaaSamples,
        {colorFormat},
        {
            {
                .blendEnable         = VK_FALSE,
                .srcColorBlendFactor = VK_BLEND_FACTOR_ZERO,
                .dstColorBlendFactor = VK_BLEND_FACTOR_ZERO,
                .colorBlendOp        = VK_BLEND_OP_ADD,
                .srcAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
                .dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
                .alphaBlendOp        = VK_BLEND_OP_ADD,
                .colorWriteMask      = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT |
                                  VK_COLOR_COMPONENT_A_BIT,
            },
        },
        {layout},
    };

    m_graphicsPipeline = GraphicsPipeline{
        m_ctx,
        m_vertexShader,
        m_fragmentShader,
        msaaSamples,
        {colorFormat},
        {
            {
                .blendEnable         = VK_FALSE,
                .srcColorBlendFactor = VK_BLEND_FACTOR_ZERO,
                .dstColorBlendFactor = VK_BLEND_FACTOR_ZERO,
                .colorBlendOp        = VK_BLEND_OP_ADD,
                .srcAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
                .dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
                .alphaBlendOp        = VK_BLEND_OP_ADD,
                .colorWriteMask      = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT |
                                  VK_COLOR_COMPONENT_A_BIT,
            },
        },
        {layout},
    };

    m_editorPipeline = GraphicsPipeline{
        m_ctx,
        m_editorVertexShader,
        m_editorFragmentShader,
        1,
        {(VkFormat)idFormat},
        {{.blendEnable = VK_FALSE, .colorWriteMask = VK_COLOR_COMPONENT_R_BIT}},
        {layout},
    };
}

void Renderer::setupShadowPass() {
    m_shadowPass = {};

    // setup pipeline
    m_shadowPass.setGraphicsPipeline(m_directionalShadowMapPipeline);
    // setup sync
    m_shadowPass.addImageMemoryBarrier({
        .sType               = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
        .srcStageMask        = VK_PIPELINE_STAGE_NONE,
        .srcAccessMask       = VK_ACCESS_NONE,
        .dstStageMask        = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT,
        .dstAccessMask       = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
        .oldLayout           = VK_IMAGE_LAYOUT_UNDEFINED,
        .newLayout           = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL,
        .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .image               = m_directionalShadowMapTexture.imageHandle(),
        .subresourceRange =
            {
                .aspectMask     = VK_IMAGE_ASPECT_DEPTH_BIT,
                .baseMipLevel   = 0,
                .levelCount     = 1,
                .baseArrayLayer = 0,
                .layerCount     = 1,
            },
    });
    m_shadowPass.addImageMemoryBarrier({
        .sType               = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
        .srcStageMask        = VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT,
        .srcAccessMask       = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
        .dstStageMask        = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
        .dstAccessMask       = VK_ACCESS_SHADER_READ_BIT,
        .oldLayout           = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL,
        .newLayout           = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
        .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .image               = m_directionalShadowMapTexture.imageHandle(),
        .subresourceRange =
            {
                .aspectMask     = VK_IMAGE_ASPECT_DEPTH_BIT,
                .baseMipLevel   = 0,
                .levelCount     = 1,
                .baseArrayLayer = 0,
                .layerCount     = 1,
            },
    });
    // setup depth attachment
    m_shadowPass.setDepthAttachment({
        .sType       = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
        .imageView   = m_directionalShadowMapTexture.imageViewHandle(),
        .imageLayout = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL,
        .resolveMode = VK_RESOLVE_MODE_NONE,
        .loadOp      = VK_ATTACHMENT_LOAD_OP_CLEAR,
        .storeOp     = VK_ATTACHMENT_STORE_OP_STORE,
        .clearValue  = {.depthStencil = {1.0f, 0}}, // Clear to far plane
    });
    // setup render area
    m_shadowPass.setRenderArea({
        .offset = {0, 0},
        .extent =
            {
                .width  = (uint32)m_directionalShadowMapTexture.image().extent().x,
                .height = (uint32)m_directionalShadowMapTexture.image().extent().y,
            },
    });
}

void Renderer::setupMainPasses() {
    m_mainPasses.clear();

    uint32 maxFrames = m_ctx.maxFramesInFlight();

    m_mainPasses.resize(maxFrames);

    for (uint32 i = 0; i < maxFrames; i++) {
        // setup pipeline
        m_mainPasses[i].setGraphicsPipeline(m_graphicsPipeline);
        m_mainPasses[i].setCubemapPipeline(m_cubemapPipeline);
        m_mainPasses[i].setCubemapTextureSlot(m_cubemapTextureBinding);
        // setup sync
        m_mainPasses[i].addImageMemoryBarrier({
            .sType               = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
            .srcStageMask        = VK_PIPELINE_STAGE_NONE,
            .srcAccessMask       = VK_ACCESS_NONE,
            .dstStageMask        = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            .dstAccessMask       = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
            .oldLayout           = VK_IMAGE_LAYOUT_UNDEFINED,
            .newLayout           = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
            .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .image               = m_colorImages[i].imageHandle(), // MSAA image
            .subresourceRange =
                {
                    .aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT,
                    .baseMipLevel   = 0,
                    .levelCount     = 1,
                    .baseArrayLayer = 0,
                    .layerCount     = 1,
                },
        });
        m_mainPasses[i].addImageMemoryBarrier({
            .sType               = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
            .srcStageMask        = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            .srcAccessMask       = VK_ACCESS_NONE,
            .dstStageMask        = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            .dstAccessMask       = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
            .oldLayout           = VK_IMAGE_LAYOUT_UNDEFINED,
            .newLayout           = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
            .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .image               = m_swapchain.images()[i],
            .subresourceRange =
                {
                    .aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT,
                    .baseMipLevel   = 0,
                    .levelCount     = 1,
                    .baseArrayLayer = 0,
                    .layerCount     = 1,
                },
        });
        m_mainPasses[i].addImageMemoryBarrier({
            .sType               = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
            .srcStageMask        = VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT,
            .srcAccessMask       = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
            .dstStageMask        = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
            .dstAccessMask       = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
            .oldLayout           = VK_IMAGE_LAYOUT_UNDEFINED,
            .newLayout           = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
            .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .image               = m_depthImagesMSAA[i].imageHandle(),
            .subresourceRange =
                {
                    .aspectMask     = VK_IMAGE_ASPECT_DEPTH_BIT,
                    .baseMipLevel   = 0,
                    .levelCount     = 1,
                    .baseArrayLayer = 0,
                    .layerCount     = 1,
                },
        });
        // setup color attachments
        m_mainPasses[i].addColorAttachment({
            // MSAA color attachment
            .sType              = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
            .imageView          = m_colorImages[i].imageViewHandle(),
            .imageLayout        = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
            .resolveMode        = VK_RESOLVE_MODE_AVERAGE_BIT,
            .resolveImageView   = m_swapchain.imageViews()[i],
            .resolveImageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
            .loadOp             = VK_ATTACHMENT_LOAD_OP_CLEAR,
            .storeOp            = VK_ATTACHMENT_STORE_OP_STORE,
            .clearValue         = {{0.0f, 0.0f, 0.0f, 1.0f}},
        });
        // setup depth attachment
        m_mainPasses[i].setDepthAttachment({
            .sType       = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
            .imageView   = m_depthImagesMSAA[i].imageViewHandle(),
            .imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
            .loadOp      = VK_ATTACHMENT_LOAD_OP_CLEAR,
            .storeOp     = VK_ATTACHMENT_STORE_OP_DONT_CARE,
            .clearValue  = {1.0f, 0},
        });
        // setup render area
        m_mainPasses[i].setRenderArea({
            .offset = {0, 0},
            .extent = m_swapchain.extent(),
        });
    }
}

void Renderer::setupEditorPasses() {
    m_editorPasses.clear();

    uint32 maxFrames = m_ctx.maxFramesInFlight();

    m_editorPasses.resize(maxFrames);

    for (uint32 i = 0; i < maxFrames; i++) {
        // setup pipeline
        m_editorPasses[i].setGraphicsPipeline(m_editorPipeline);
        // setup sync
        m_editorPasses[i].addImageMemoryBarrier({
            .sType               = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
            .srcStageMask        = VK_PIPELINE_STAGE_NONE,
            .srcAccessMask       = VK_ACCESS_NONE,
            .dstStageMask        = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            .dstAccessMask       = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
            .oldLayout           = VK_IMAGE_LAYOUT_UNDEFINED,
            .newLayout           = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
            .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .image               = m_idImages[i].imageHandle(),
            .subresourceRange =
                {
                    .aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT,
                    .baseMipLevel   = 0,
                    .levelCount     = 1,
                    .baseArrayLayer = 0,
                    .layerCount     = 1,
                },
        });
        m_editorPasses[i].addImageMemoryBarrier({
            .sType               = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
            .srcStageMask        = VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT,
            .srcAccessMask       = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
            .dstStageMask        = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
            .dstAccessMask       = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
            .oldLayout           = VK_IMAGE_LAYOUT_UNDEFINED,
            .newLayout           = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
            .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .image               = m_depthImages[i].imageHandle(),
            .subresourceRange =
                {
                    .aspectMask     = VK_IMAGE_ASPECT_DEPTH_BIT,
                    .baseMipLevel   = 0,
                    .levelCount     = 1,
                    .baseArrayLayer = 0,
                    .layerCount     = 1,
                },
        });
        // setup color attachment
        m_editorPasses[i].addColorAttachment({
            .sType       = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
            .imageView   = m_idImages[i].imageViewHandle(),
            .imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
            .loadOp      = VK_ATTACHMENT_LOAD_OP_CLEAR,
            .storeOp     = VK_ATTACHMENT_STORE_OP_STORE,
            .clearValue  = {.color = {.uint32 = {0xFFFFFFFF, 0, 0, 0}}}, // clear to id -1
        });
        // setup depth attachment
        m_editorPasses[i].setDepthAttachment({
            .sType       = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
            .imageView   = m_depthImages[i].imageViewHandle(),
            .imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
            .loadOp      = VK_ATTACHMENT_LOAD_OP_CLEAR,
            .storeOp     = VK_ATTACHMENT_STORE_OP_DONT_CARE,
            .clearValue  = {1.0f, 0},
        });
        m_editorPasses[i].setRenderArea({
            .offset = {0, 0},
            .extent = m_swapchain.extent(),
        });
    }
}

void Renderer::handleMouseHover(CommandBuffer& cmd, uint32 imageIndex, int32 posX, int32 posY) {
    bool outOfBounds = posX < 0 || posY < 0 || //
                       posX >= static_cast<int32>(m_swapchain.extent().width) ||
                       posY >= static_cast<int32>(m_swapchain.extent().height);

    if (!outOfBounds) {
        const VkImageMemoryBarrier2 barrier = {
            .sType               = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
            .srcStageMask        = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
            .srcAccessMask       = VK_ACCESS_NONE,
            .dstStageMask        = VK_PIPELINE_STAGE_2_COPY_BIT,
            .dstAccessMask       = VK_ACCESS_TRANSFER_READ_BIT,
            .oldLayout           = VK_IMAGE_LAYOUT_UNDEFINED,
            .newLayout           = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
            .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .image               = m_idImages[imageIndex].imageHandle(),
            .subresourceRange =
                {
                    .aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT,
                    .baseMipLevel   = 0,
                    .levelCount     = 1,
                    .baseArrayLayer = 0,
                    .layerCount     = 1,
                },
        };
        cmd.pipelineBarrier({
            .sType                   = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
            .imageMemoryBarrierCount = 1,
            .pImageMemoryBarriers    = &barrier,
        });

        const VkBufferImageCopy2 bufferImageRegion = {
            .sType             = VK_STRUCTURE_TYPE_BUFFER_IMAGE_COPY_2,
            .bufferOffset      = 0,
            .bufferRowLength   = 0,
            .bufferImageHeight = 0,
            .imageSubresource =
                {
                    .aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT,
                    .mipLevel       = 0,
                    .baseArrayLayer = 0,
                    .layerCount     = 1,
                },
            .imageOffset = {.x = posX, .y = posY, .z = 0},
            .imageExtent = {.width = 1, .height = 1, .depth = 1},
        };
        cmd.copyImageToBuffer({
            .sType          = VK_STRUCTURE_TYPE_COPY_IMAGE_TO_BUFFER_INFO_2,
            .srcImage       = m_idImages[imageIndex].imageHandle(),
            .srcImageLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
            .dstBuffer      = m_idReadbackBuffers[imageIndex].bufferHandle(),
            .regionCount    = 1,
            .pRegions       = &bufferImageRegion,
        });

        // transition back to color attachment optimal
        const VkImageMemoryBarrier2 postBarrier = {
            .sType               = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
            .srcStageMask        = VK_PIPELINE_STAGE_2_COPY_BIT,
            .srcAccessMask       = VK_ACCESS_TRANSFER_READ_BIT,
            .dstStageMask        = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            .dstAccessMask       = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
            .oldLayout           = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
            .newLayout           = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
            .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .image               = m_idImages[imageIndex].imageHandle(),
            .subresourceRange =
                {
                    .aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT,
                    .baseMipLevel   = 0,
                    .levelCount     = 1,
                    .baseArrayLayer = 0,
                    .layerCount     = 1,
                },
        };
        cmd.pipelineBarrier({
            .sType                   = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
            .imageMemoryBarrierCount = 1,
            .pImageMemoryBarriers    = &postBarrier,
        });

        m_pendingReadback = true;
    } else {
        m_hoveredEntityID = entt::null;
    }
}

void Renderer::transitionAttachmentsForPresent(CommandBuffer& cmd, uint32 imageIndex) {
    const VkImageMemoryBarrier2 barriers[] = {
        // Transition swapchain image for presentation
        {
            .sType               = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
            .srcStageMask        = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            .srcAccessMask       = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
            .dstStageMask        = VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT,
            .dstAccessMask       = VK_ACCESS_NONE,
            .oldLayout           = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
            .newLayout           = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
            .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .image               = m_swapchain.images()[imageIndex],
            .subresourceRange    = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1},
        },
        // Transition idImage for presentation
        {
            .sType               = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
            .srcStageMask        = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            .srcAccessMask       = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
            .dstStageMask        = VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT,
            .dstAccessMask       = VK_ACCESS_NONE,
            .oldLayout           = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
            .newLayout           = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
            .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .image               = m_idImages[imageIndex].imageHandle(),
            .subresourceRange    = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1},
        },
        // MSAA color image should be transitioned to a safe state too
        {
            .sType               = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
            .srcStageMask        = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            .srcAccessMask       = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
            .dstStageMask        = VK_PIPELINE_STAGE_NONE,
            .dstAccessMask       = VK_ACCESS_NONE,
            .oldLayout           = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
            .newLayout           = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
            .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .image               = m_colorImages[imageIndex].imageHandle(),
            .subresourceRange    = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1},
        },
    };
    const VkDependencyInfo dependencyInfo = {
        .sType                   = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
        .imageMemoryBarrierCount = static_cast<uint32>(std::size(barriers)),
        .pImageMemoryBarriers    = barriers,
    };
    cmd.pipelineBarrier(dependencyInfo);
}

void Renderer::handleWindowResize() {
    m_ctx.waitIdle();

    ivec2 framebufferSize = m_window.framebufferSize();
    if (framebufferSize.x == 0 || framebufferSize.y == 0) {
        return; // Minimized, skip for now
    }

    m_swapchain.recreate(m_ctx, framebufferSize);

    // recreate attachments
    uint32 msaaSamples = m_ctx.queryMaxUsableSampleCount();
    usize3 extent      = {m_swapchain.extent().width, m_swapchain.extent().height, 1};

    m_colorImages.clear();
    m_depthImagesMSAA.clear();
    m_depthImages.clear();
    m_idImages.clear();

    const Format colorFormat = Format(m_ctx.swapchainFormat());
    const Format depthFormat = Format(m_ctx.queryDepthFormat());
    const Format idFormat    = Format::R32_UINT;

    uint32 maxFrames = m_ctx.maxFramesInFlight();
    for (uint32 i = 0; i < maxFrames; i++) {
        // main pass
        m_colorImages.emplace_back(extent, 1, msaaSamples, ImageUsage::ColorAttachment, colorFormat);
        m_depthImagesMSAA.emplace_back(extent, 1, msaaSamples, ImageUsage::DepthStencilAttachment, depthFormat);
        // editor pass
        m_idImages.emplace_back(extent, 1, 1, ImageUsage::ColorAttachment | ImageUsage::TransferSrc, idFormat);
        m_depthImages.emplace_back(extent, 1, 1, ImageUsage::DepthStencilAttachment, depthFormat);
    }

    setupShadowPass();
    setupMainPasses();
    setupEditorPasses();
}

void Renderer::writeDescriptorSetsHelper(uint32 frameIndex, uint32 numLights) {
    std::vector<VkWriteDescriptorSet> descriptorWrites;
    // MVP buffer
    const VkDescriptorBufferInfo uboBufferInfo = {
        .buffer = m_ubos[frameIndex].bufferHandle(),
        .offset = 0,
        .range  = sizeof(PBRVertexUniformBufferObject),
    };
    descriptorWrites.push_back(VkWriteDescriptorSet{
        .sType            = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
        .dstSet           = m_ctx.descriptorSet(frameIndex).descriptorSet(),
        .dstBinding       = 0,
        .dstArrayElement  = 0,
        .descriptorCount  = 1,
        .descriptorType   = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        .pImageInfo       = nullptr,
        .pBufferInfo      = &uboBufferInfo,
        .pTexelBufferView = nullptr,
    });

    // Storage buffer - Lights
    const VkDescriptorBufferInfo ssboBufferInfo = {
        .buffer = m_lights[frameIndex].bufferHandle(),
        .offset = 0,
        .range  = sizeof(PointLightShaderObject) * numLights,
    };
    if (numLights > 0) {
        descriptorWrites.push_back(VkWriteDescriptorSet{
            .sType            = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
            .dstSet           = m_ctx.descriptorSet(frameIndex).descriptorSet(),
            .dstBinding       = 3,
            .dstArrayElement  = 0,
            .descriptorCount  = 1,
            .descriptorType   = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
            .pImageInfo       = nullptr,
            .pBufferInfo      = &ssboBufferInfo,
            .pTexelBufferView = nullptr,
        });
    }

    // Shadow sampler
    const VkDescriptorImageInfo shadowImageInfo = {
        .sampler     = m_directionalShadowMapTexture.samplerHandle(),
        .imageView   = m_directionalShadowMapTexture.imageViewHandle(),
        .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
    };
    descriptorWrites.push_back(VkWriteDescriptorSet{
        .sType            = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
        .dstSet           = m_ctx.descriptorSet(frameIndex).descriptorSet(),
        .dstBinding       = 5,
        .dstArrayElement  = 0,
        .descriptorCount  = 1,
        .descriptorType   = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
        .pImageInfo       = &shadowImageInfo,
        .pBufferInfo      = nullptr,
        .pTexelBufferView = nullptr,
    });

    m_ctx.descriptorSet(frameIndex).write(descriptorWrites);
}

uint32 Renderer::updateLights(uint32 frameIndex) {
    uint32 numLights = 0;
    GWorld()->registry().view<LightComponent>().each([&](const LightComponent& light) {
        if (numLights >= 256) {
            return; // Max lights reached
        }
        PointLightShaderObject lightShaderObject = {
            .position  = light.position,
            .color     = light.color,
            .intensity = light.intensity,
        };
        m_lights[frameIndex].copy(&lightShaderObject, sizeof(lightShaderObject) * numLights, sizeof(lightShaderObject));
        numLights++;
    });
    return numLights;
}

} // namespace R3::vulkan

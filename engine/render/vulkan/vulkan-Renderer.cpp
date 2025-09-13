#include "vulkan-Renderer.hpp"

#include <array>
#include <filesystem>
#include <format>
#include <iterator>
#include <span>
#include <vector>
#include <vulkan/vulkan.h>
#include <entt/entity/registry.hpp>
#include <entt/entity/view.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "api/Exception.hpp"
#include "api/Types.hpp"
#include "components/LightComponent.hpp"
#include "core/Camera.hpp"
#include "core/Engine.hpp"
#include "core/ResourceManager.hpp"
#include "core/World.hpp"
#include "input/InputCodes.hpp"
#include "passes/vulkan-EditorPass.hpp"
#include "passes/vulkan-MainPass.hpp"
#include "passes/vulkan-ShadowPass.hpp"
#include "render/Buffer.hpp"
#include "render/Flags.hpp"
#include "render/Image.hpp"
#include "render/ShaderObjects.hpp"
#include "render/Texture.hpp"
#include "render/Window.hpp"
#include "vulkan-CommandBuffer.hpp"
#include "vulkan-DescriptorSet.hpp"
#include "vulkan-GraphicsPipeline.hpp"
#include "vulkan-RenderContext.hpp"
#include "vulkan-RenderPass.hpp"
#include "vulkan-Shader.hpp"
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
    m_swapchain      = Swapchain{m_ctx, m_window.framebufferSize()};
    uint32 maxFrames = m_ctx.maxFramesInFlight();

    //--- Color/Depth Image
    uint32 msaaSamples = m_ctx.queryMaxUsableSampleCount();

    usize3 extent   = {m_swapchain.extent().width, m_swapchain.extent().height, 1};
    Format idFormat = Format::R32_UINT;

    for (uint32 i = 0; i < maxFrames; i++) {
        m_colorImages.emplace_back(extent, 1, msaaSamples, ImageUsage::ColorAttachment, Format(m_swapchain.format()));
        m_depthImages.emplace_back(
            extent, 1, msaaSamples, ImageUsage::DepthStencilAttachment, Format(m_ctx.queryDepthFormat()));
        m_depthImages1Bit.emplace_back(
            extent, 1, 1, ImageUsage::DepthStencilAttachment, Format(m_ctx.queryDepthFormat()));
        m_idImages.emplace_back(extent, 1, 1, ImageUsage::ColorAttachment | ImageUsage::TransferSrc, idFormat);
    }

    //--- Shaders
    m_vertexShader   = Shader{m_ctx, "_spirv/pbr.vert.spv", VK_SHADER_STAGE_VERTEX_BIT};
    m_fragmentShader = Shader{m_ctx, "_spirv/pbr.frag.spv", VK_SHADER_STAGE_FRAGMENT_BIT};

    m_cubemapVertexShader   = Shader{m_ctx, "_spirv/cubemap.vert.spv", VK_SHADER_STAGE_VERTEX_BIT};
    m_cubemapFragmentShader = Shader{m_ctx, "_spirv/cubemap.frag.spv", VK_SHADER_STAGE_FRAGMENT_BIT};

    m_directionalShadowMapVertexShader =
        Shader{m_ctx, "_spirv/directional_shadow_map.vert.spv", VK_SHADER_STAGE_VERTEX_BIT};
    m_directionalShadowMapFragmentShader =
        Shader{m_ctx, "_spirv/directional_shadow_map.frag.spv", VK_SHADER_STAGE_FRAGMENT_BIT};

    m_editorVertexShader   = Shader{m_ctx, "_spirv/editor.vert.spv", VK_SHADER_STAGE_VERTEX_BIT};
    m_editorFragmentShader = Shader{m_ctx, "_spirv/editor.frag.spv", VK_SHADER_STAGE_FRAGMENT_BIT};

    //--- Graphics Pipeline
    const VkDescriptorSetLayout layout = ctx.descriptorLayout();

    const VkFormat colorFormat = m_swapchain.format();

    m_directionalShadowMapPipeline = GraphicsPipeline{
        m_ctx,
        {m_directionalShadowMapVertexShader, m_directionalShadowMapFragmentShader},
        VK_SAMPLE_COUNT_1_BIT,
        {},
        {},
        {layout},
        {{.stageFlags = VK_SHADER_STAGE_VERTEX_BIT, .offset = 0, .size = sizeof(ShadowVertexPushConstants)}},
        {Vertex::getBindingDescription()},
        {Vertex::getAttributeDescriptions()},
    };

    m_cubemapPipeline = GraphicsPipeline{
        m_ctx,
        {m_cubemapVertexShader, m_cubemapFragmentShader},
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
        {{.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT, .size = sizeof(CubemapFragmentPushConstants)}},
    };

    m_graphicsPipeline = GraphicsPipeline{
        m_ctx,
        {m_vertexShader, m_fragmentShader},
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
        {
            {
                .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
                .offset     = 0,
                .size       = sizeof(PBRVertexPushConstants),
            },
            {
                .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
                .offset     = sizeof(PBRVertexPushConstants),
                .size       = sizeof(PBRFragmentPushConstants),
            },
        },
        {Vertex::getBindingDescription()},
        {Vertex::getAttributeDescriptions()},
    };

    m_editorPipeline = GraphicsPipeline{
        m_ctx,
        {m_editorVertexShader, m_editorFragmentShader},
        1,
        {(VkFormat)idFormat},
        {{.blendEnable = VK_FALSE, .colorWriteMask = VK_COLOR_COMPONENT_R_BIT}},
        {layout},
        {
            {
                .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
                .offset     = 0,
                .size       = sizeof(EditorVertexPushConstants),
            },
            {
                .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
                .offset     = sizeof(EditorVertexPushConstants),
                .size       = sizeof(EditorFragmentPushConstants),
            },
        },
        {Vertex::getBindingDescription()},
        {Vertex::getAttributeDescriptions()},
    };

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
        m_hoveredEntityIDs.push_back(0xFFFF'FFFF);
    }

    GWorld()->camera().setActive(true);
}

Renderer::~Renderer() noexcept {
    m_ctx.waitIdle();
}

void Renderer::draw() {
    // Handle m_window resize
    if (m_window.shouldResize()) {
        handleWindowResize();
        m_window.setShouldResize(false);
    }

    // Get current frame index
    uint32 currFrame = m_ctx.currentFrameIndex();
    m_ctx.waitForFrame(currFrame);

    // Acquire next image
    uint32 imageIndex;
    VkResult result = m_swapchain.acquireNextImage(m_ctx.imageAvailableSemaphore(currFrame), imageIndex);
    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        m_window.setShouldResize(true);
        return;
    } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        throw Exception{std::format("Failed to acquire swap chain image: {}", static_cast<int>(result))};
    }

    // update view projection matrices in ubo
    GWorld()->camera().applyPerspective(m_window.aspectRatio(), m_window.size(), m_ubo.view, m_ubo.projection);
    m_ubo.lightViewProjection = lightSpaceMatrix;
    m_ubos[currFrame].copy(&m_ubo, 0, sizeof(m_ubo));

    // update lights
    uint32 numLights = updateLights(currFrame);
    fvec3 lightPos;
    GWorld()->registry().view<LightComponent>().each([&](const LightComponent& light) { lightPos = light.position; });

    writeDescriptorSetsHelper(currFrame, numLights);

    CommandBuffer& cmd = m_ctx.graphicsCommandBuffer(currFrame);
    cmd.reset();
    cmd.begin();

    VkDescriptorSet descriptorSet = m_ctx.descriptorSet(currFrame).descriptorSet();

    // shadow pass
    m_shadowPass.setDescriptorSet(descriptorSet);
    m_shadowPass.setLightSpaceMatrix(lightSpaceMatrix);
    m_shadowPass.execute(cmd);

    // main pass
    m_mainPasses[imageIndex].setDescriptorSet(descriptorSet);
    m_mainPasses[imageIndex].setLightCount(numLights);
    m_mainPasses[imageIndex].setSelectedEntityID(m_selectedEntityID);
    m_mainPasses[imageIndex].execute(cmd);

    handleMouseHover(cmd, imageIndex);
    if (GWindow()->mouseButtonPressed(MouseButton::Left)) {
        handleMouseClick(cmd, imageIndex);
    }

#if R3_EDITOR
    // editor pass
    m_editorPasses[imageIndex].setDescriptorSet(descriptorSet);
    m_editorPasses[imageIndex].execute(cmd);
#endif

    transitionAttachmentsForPresent(cmd, imageIndex);
    cmd.end();

    // Submit command buffer - use per-frame acquire, per-image render finished
    m_ctx.submit(m_ctx.graphicsQueue(),
                 cmd.commandBuffer(),
                 VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                 m_ctx.imageAvailableSemaphore(currFrame),
                 m_ctx.renderFinishedSemaphore(currFrame),
                 m_ctx.inFlightFence(currFrame));

    // Present - use per-image semaphore
    const VkSemaphore signalSemaphores[] = {m_ctx.renderFinishedSemaphore(currFrame)};
    const VkSwapchainKHR swapchains[]    = {m_swapchain.swapchain()};
    const VkPresentInfoKHR presentInfo{
        .sType              = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
        .waitSemaphoreCount = 1,
        .pWaitSemaphores    = signalSemaphores,
        .swapchainCount     = 1,
        .pSwapchains        = swapchains,
        .pImageIndices      = &imageIndex,
        .pResults           = nullptr,
    };
    result = vkQueuePresentKHR(m_ctx.graphicsQueue(), &presentInfo);
    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
        m_window.setShouldResize(true);
    } else if (result != VK_SUCCESS) {
        throw Exception{std::format("vkQueuePresentKHR returned: {}", static_cast<int>(result))};
    }

    m_ctx.advanceFrame();
}

void Renderer::setupShadowPass() {
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
            .imageView   = m_depthImages[i].imageViewHandle(),
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
            .image               = m_depthImages1Bit[i].imageHandle(),
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
        m_editorPasses[i].setDepthAttachment({
            .sType       = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
            .imageView   = m_depthImages1Bit[i].imageViewHandle(),
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

void Renderer::handleMouseHover(CommandBuffer& cmd, uint32 imageIndex) {
    ivec2 cursorPos = static_cast<ivec2>(GWindow()->cursorPosition());

    bool outOfBounds = cursorPos.x < 0 || cursorPos.y < 0 ||
                       cursorPos.x >= static_cast<int32>(m_swapchain.extent().width) ||
                       cursorPos.y >= static_cast<int32>(m_swapchain.extent().height);

    if (!outOfBounds) {
        m_hoveredEntityIDs[imageIndex] = *((uint32*)m_idReadbackBuffers[imageIndex].data());

        const VkImageMemoryBarrier2 barrier = {
            .sType               = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
            .srcStageMask        = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
            .srcAccessMask       = VK_ACCESS_NONE,
            .dstStageMask        = VK_PIPELINE_STAGE_TRANSFER_BIT,
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
            .imageSubresource  = {.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT,
                                  .mipLevel       = 0,
                                  .baseArrayLayer = 0,
                                  .layerCount     = 1},
            .imageOffset       = {.x = int32(cursorPos.x), .y = int32(cursorPos.y), .z = 0},
            .imageExtent       = {.width = 1, .height = 1, .depth = 1},
        };
        cmd.copyImageToBuffer({
            .sType          = VK_STRUCTURE_TYPE_COPY_IMAGE_TO_BUFFER_INFO_2,
            .srcImage       = m_idImages[imageIndex].imageHandle(),
            .srcImageLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
            .dstBuffer      = m_idReadbackBuffers[imageIndex].bufferHandle(),
            .regionCount    = 1,
            .pRegions       = &bufferImageRegion,
        });
    } else {
        m_hoveredEntityIDs[imageIndex] = 0xFFFF'FFFF;
    }
}

void Renderer::handleMouseClick(CommandBuffer& cmd, uint32 imageIndex) {
    m_selectedEntityID = m_hoveredEntityIDs[imageIndex];
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
    m_depthImages.clear();
    m_depthImages1Bit.clear();
    m_idImages.clear();

    uint32 maxFrames = m_ctx.maxFramesInFlight();
    for (uint32 i = 0; i < maxFrames; i++) {
        m_colorImages.emplace_back(extent, 1, msaaSamples, ImageUsage::ColorAttachment, Format(m_swapchain.format()));
        m_depthImages.emplace_back(
            extent, 1, msaaSamples, ImageUsage::DepthStencilAttachment, Format(m_ctx.queryDepthFormat()));
        m_depthImages1Bit.emplace_back(
            extent, 1, 1, ImageUsage::DepthStencilAttachment, Format(m_ctx.queryDepthFormat()));
        m_idImages.emplace_back(extent, 1, 1, ImageUsage::ColorAttachment | ImageUsage::TransferSrc, Format::R32_UINT);
    }

    m_shadowPass = {};
    m_mainPasses.clear();
    m_editorPasses.clear();

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

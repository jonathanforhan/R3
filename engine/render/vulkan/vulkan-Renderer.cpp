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
#include <entt/resource/resource.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "api/Exception.hpp"
#include "api/Types.hpp"
#include "components/LightComponent.hpp"
#include "components/MaterialComponent.hpp"
#include "components/MeshComponent.hpp"
#include "components/TransformComponent.hpp"
#include "core/Camera.hpp"
#include "core/Engine.hpp"
#include "core/ResourceManager.hpp"
#include "core/World.hpp"
#include "engine/editor/Editor.hpp"
#include "render/Buffer.hpp"
#include "render/Flags.hpp"
#include "render/Image.hpp"
#include "render/ShaderObjects.hpp"
#include "render/Texture.hpp"
#include "render/Window.hpp"
#include "vulkan-Check.hpp"
#include "vulkan-CommandBuffer.hpp"
#include "vulkan-DescriptorSet.hpp"
#include "vulkan-GraphicsPipeline.hpp"
#include "vulkan-RenderContext.hpp"
#include "vulkan-Shader.hpp"
#include "vulkan-Swapchain.hpp"

namespace R3::vulkan {

static glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 1.0f, 7.5f);
static glm::mat4 lightView =
    glm::lookAt(glm::vec3(-2.0f, 4.0f, -4.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
static glm::mat4 lightSpaceMatrix = lightProjection * lightView;

Renderer::Renderer(Window& window, RenderContext& ctx)
    : m_window(window),
      m_ctx{ctx} {
    //--- Swapchain
    //    - images
    //    - image views
    m_swapchain = Swapchain{m_ctx, m_window.framebufferSize()};

    //--- Color/Depth Image
    auto msaaSamples = m_ctx.queryMaxUsableSampleCount();

    usize3 extent = {m_swapchain.extent().width, m_swapchain.extent().height, 1};

    m_colorImage = Image{extent, 1, msaaSamples, ImageUsage::ColorAttachment, Format(m_swapchain.format())};
    m_depthImage = Image{extent, 1, msaaSamples, ImageUsage::DepthStencilAttachment, Format(m_ctx.queryDepthFormat())};

    //--- Shaders
    m_vertexShader   = Shader{m_ctx, "_spirv/pbr.vert.spv", VK_SHADER_STAGE_VERTEX_BIT};
    m_fragmentShader = Shader{m_ctx, "_spirv/pbr.frag.spv", VK_SHADER_STAGE_FRAGMENT_BIT};

    m_cubemapVertexShader   = Shader{m_ctx, "_spirv/cubemap.vert.spv", VK_SHADER_STAGE_VERTEX_BIT};
    m_cubemapFragmentShader = Shader{m_ctx, "_spirv/cubemap.frag.spv", VK_SHADER_STAGE_FRAGMENT_BIT};

    m_directionalShadowMapVertexShader =
        Shader{m_ctx, "_spirv/directional_shadow_map.vert.spv", VK_SHADER_STAGE_VERTEX_BIT};
    m_directionalShadowMapFragmentShader =
        Shader{m_ctx, "_spirv/directional_shadow_map.frag.spv", VK_SHADER_STAGE_FRAGMENT_BIT};

    //--- Graphics Pipeline
    const VkDescriptorSetLayout layout = ctx.descriptorLayout();

    const VkFormat colorFormat = m_swapchain.format();

    m_graphicsPipeline = GraphicsPipeline{
        m_ctx,
        {m_vertexShader, m_fragmentShader},
        msaaSamples,
        {colorFormat},
        {layout},
        {
            {
                .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
                .offset     = 0,
                .size       = sizeof(VertexPushConstants),
            },
            {
                .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
                .offset     = sizeof(VertexPushConstants),
                .size       = sizeof(FragmentPushConstants),
            },
        },
        {Vertex::getBindingDescription()},
        {Vertex::getAttributeDescriptions()},
    };

    m_cubemapPipeline = GraphicsPipeline{
        m_ctx,
        {m_cubemapVertexShader, m_cubemapFragmentShader},
        msaaSamples,
        {colorFormat},
        {layout},
        {{.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT, .size = sizeof(FragmentPushConstantsCubemap)}},
    };

    m_directionalShadowMapPipeline = GraphicsPipeline{
        m_ctx,
        {m_directionalShadowMapVertexShader, m_directionalShadowMapFragmentShader},
        VK_SAMPLE_COUNT_1_BIT,
        {},
        {layout},
        {{.stageFlags = VK_SHADER_STAGE_VERTEX_BIT, .offset = 0, .size = sizeof(VertexPushConstantsShadow)}},
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
        ubo = Buffer{sizeof(VertexUniformBufferObject), BufferUsage::HostUniform};
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

    GWorld()->camera().setActive(true);
} // namespace R3::vulkan

Renderer::~Renderer() noexcept {
    m_ctx.waitIdle();
}

void Renderer::draw() {
    // Handle m_window resize
    if (m_window.shouldResize()) {
        handleWindowResize();
        m_window.setShouldResize(false);
        return;
    }

    // Get current frame index
    uint32 currFrame = m_ctx.currentFrameIndex();
    m_ctx.waitForFrame(currFrame);

    // Acquire next image
    uint32 imageIndex;
    VkResult result = m_swapchain.acquireNextImage(m_ctx.imageAvailableSemaphore(currFrame), imageIndex);
    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        m_window.setShouldResize(true);
    } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        throw Exception{std::format("Failed to acquire swap chain image: {}", static_cast<int>(result))};
    }

    // update view projection matrices in ubo
    GWorld()->camera().applyPerspective(m_window.aspectRatio(), m_window.size(), m_ubo.view, m_ubo.projection);
    m_ubo.lightViewProjection = lightSpaceMatrix;
    m_ubos[currFrame].copy(&m_ubo, 0, sizeof(m_ubo));

    uint32 numLights = updateLights(currFrame);

    fvec3 lightPos;
    GWorld()->registry().view<LightComponent>().each([&](const LightComponent& light) { lightPos = light.position; });

    writeDescriptorSetsHelper(currFrame, numLights);

    CommandBuffer& cmd = m_ctx.graphicsCommandBuffer(currFrame);
    cmd.reset();

    cmd.begin();

    shadowPass(cmd, currFrame);

    transitionAttachmentsForRender(cmd, imageIndex);
    beginRenderingHelper(cmd, currFrame);

    cubemapPass(cmd, imageIndex);

    VkDescriptorSet descriptorSets[] = {m_ctx.descriptorSet(currFrame).descriptorSet()};

    // rest of scene
    bindPipelineHelper(cmd, m_graphicsPipeline);
    cmd.bindDescriptorSets({
        .sType              = VK_STRUCTURE_TYPE_BIND_DESCRIPTOR_SETS_INFO,
        .stageFlags         = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
        .layout             = m_graphicsPipeline.layout(),
        .firstSet           = 0,
        .descriptorSetCount = static_cast<uint32>(std::size(descriptorSets)),
        .pDescriptorSets    = descriptorSets,
    });

    GWorld()->registry().view<MeshComponent, MaterialComponent, TransformComponent>().each(
        [&](const MeshComponent& mesh, const MaterialComponent& mat, const TransformComponent& trans) {
            const VertexPushConstants vertPushConstants = {
                .model = trans.transform(),
            };
            cmd.pushConstants({
                .sType      = VK_STRUCTURE_TYPE_PUSH_CONSTANTS_INFO,
                .layout     = m_graphicsPipeline.layout(),
                .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
                .offset     = 0,
                .size       = sizeof(VertexPushConstants),
                .pValues    = &vertPushConstants,
            });

            const FragmentPushConstants fragPushConstants = {
                .viewPosition       = GWorld()->camera().position(),
                .numLights          = numLights,
                .iAlbedo            = mat.iAlbedo,
                .iMetallicRoughness = mat.iMetallicRoughness,
                .iNormal            = mat.iNormal,
                .iAmbientOcclusion  = mat.iAmbientOcclusion,
                .iEmissive          = mat.iEmissive,
            };
            cmd.pushConstants({
                .sType      = VK_STRUCTURE_TYPE_PUSH_CONSTANTS_INFO,
                .layout     = m_graphicsPipeline.layout(),
                .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
                .offset     = sizeof(VertexPushConstants),
                .size       = sizeof(FragmentPushConstants),
                .pValues    = &fragPushConstants,
            });

            const VkBuffer vboIndices[]  = {mesh.vertexBufferIndex->bufferHandle()};
            const VkDeviceSize offsets[] = {0};
            const VkBuffer iboIndex      = mesh.indexBufferIndex->bufferHandle();
            cmd.bindVertexBuffers(0, vboIndices, offsets);
            cmd.bindIndexBuffer(iboIndex, 0, VK_INDEX_TYPE_UINT32);
            cmd.drawIndexed(static_cast<uint32>(mesh.indexCount));
        });

#if R3_EDITOR
    cmd.setDepthTestEnable(false); // Disable depth for UI
    GEngine()->m_editor->draw(cmd);
#endif

    cmd.endRendering();

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

void Renderer::shadowPass(CommandBuffer& cmd, uint32 frameIndex) {
    // First, transition the shadow cubemap for depth attachment use
    const VkImageMemoryBarrier2 shadowBarrier = {
        .sType               = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
        .srcStageMask        = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
        .srcAccessMask       = VK_ACCESS_NONE,
        .dstStageMask        = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT,
        .dstAccessMask       = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
        .oldLayout           = VK_IMAGE_LAYOUT_UNDEFINED,
        .newLayout           = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL,
        .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .image               = m_directionalShadowMapTexture.imageHandle(), // Your shadow cubemap
        .subresourceRange =
            {
                .aspectMask     = VK_IMAGE_ASPECT_DEPTH_BIT,
                .baseMipLevel   = 0,
                .levelCount     = 1,
                .baseArrayLayer = 0,
                .layerCount     = 1,
            },
    };
    const VkDependencyInfo shadowDependency = {
        .sType                   = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
        .imageMemoryBarrierCount = 1,
        .pImageMemoryBarriers    = &shadowBarrier,
    };
    cmd.pipelineBarrier(shadowDependency);

    // Begin rendering - ONLY depth attachment, no color
    const VkRenderingAttachmentInfo depthAttachment = {
        .sType              = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
        .imageView          = m_directionalShadowMapTexture.imageViewHandle(),
        .imageLayout        = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL,
        .resolveMode        = VK_RESOLVE_MODE_NONE,
        .resolveImageView   = VK_NULL_HANDLE,
        .resolveImageLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        .loadOp             = VK_ATTACHMENT_LOAD_OP_CLEAR,
        .storeOp            = VK_ATTACHMENT_STORE_OP_STORE,
        .clearValue         = {.depthStencil = {1.0f, 0}}, // Clear to far plane
    };
    const VkRenderingInfo renderingInfo = {
        .sType                = VK_STRUCTURE_TYPE_RENDERING_INFO,
        .renderArea           = {{0, 0}, {1024, 1024}}, // Shadow map resolution
        .layerCount           = 1,
        .viewMask             = 0,
        .colorAttachmentCount = 0, // NO color attachments
        .pColorAttachments    = nullptr,
        .pDepthAttachment     = &depthAttachment,
        .pStencilAttachment   = nullptr,
    };
    cmd.beginRendering(renderingInfo);
    {
        bindPipelineHelper(cmd, m_directionalShadowMapPipeline);
        cmd.setCullMode(VK_CULL_MODE_NONE);
        cmd.setViewport({
            .x        = 0.0f,
            .y        = 0.0f,
            .width    = 1024.0f,
            .height   = 1024.0f,
            .minDepth = 0.0f,
            .maxDepth = 1.0f,
        });
        cmd.setScissor({{0, 0}, {1024, 1024}});

        VkDescriptorSet descriptorSets[] = {m_ctx.descriptorSet(frameIndex).descriptorSet()};
        cmd.bindDescriptorSets({
            .sType              = VK_STRUCTURE_TYPE_BIND_DESCRIPTOR_SETS_INFO,
            .stageFlags         = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
            .layout             = m_directionalShadowMapPipeline.layout(),
            .firstSet           = 0,
            .descriptorSetCount = static_cast<uint32>(std::size(descriptorSets)),
            .pDescriptorSets    = descriptorSets,
        });

        // Render all shadow casters
        GWorld()->registry().view<MeshComponent, TransformComponent>().each([&](const auto& mesh, const auto& trans) {
            const VertexPushConstantsShadow vertPush = {
                .model         = trans.transform(),
                .lightViewProj = lightSpaceMatrix,
            };
            cmd.pushConstants({
                .sType      = VK_STRUCTURE_TYPE_PUSH_CONSTANTS_INFO,
                .layout     = m_directionalShadowMapPipeline.layout(),
                .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
                .offset     = 0,
                .size       = sizeof(VertexPushConstantsShadow),
                .pValues    = &vertPush,
            });

            // Bind mesh and draw
            const VkBuffer vboIndices[]  = {mesh.vertexBufferIndex->bufferHandle()};
            const VkDeviceSize offsets[] = {0};
            const VkBuffer iboIndex      = mesh.indexBufferIndex->bufferHandle();
            cmd.bindVertexBuffers(0, vboIndices, offsets);
            cmd.bindIndexBuffer(iboIndex, 0, VK_INDEX_TYPE_UINT32);
            cmd.drawIndexed(static_cast<uint32>(mesh.indexCount));
        });
    }
    cmd.endRendering();
    const VkImageMemoryBarrier2 barrier = {
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
        .subresourceRange    = {VK_IMAGE_ASPECT_DEPTH_BIT, 0, 1, 0, 1},
    };
    const VkDependencyInfo dependency = {
        .sType                   = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
        .imageMemoryBarrierCount = 1,
        .pImageMemoryBarriers    = &barrier,
    };
    cmd.pipelineBarrier(dependency);
}

void Renderer::cubemapPass(CommandBuffer& cmd, uint32 frameIndex) {
    // cubemap
    bindPipelineHelper(cmd, m_cubemapPipeline);
    cmd.setDepthTestEnable(false); // Disable depth for skybox

    // Push Constants
    const FragmentPushConstantsCubemap fragPushConstants = {.iCubemap = m_cubemapTextureBinding};
    cmd.pushConstants({
        .sType      = VK_STRUCTURE_TYPE_PUSH_CONSTANTS_INFO,
        .layout     = m_cubemapPipeline.layout(),
        .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
        .offset     = 0,
        .size       = sizeof(FragmentPushConstantsCubemap),
        .pValues    = &fragPushConstants,
    });

    // Descriptor Sets
    VkDescriptorSet descriptorSets[] = {m_ctx.descriptorSet(frameIndex).descriptorSet()};
    cmd.bindDescriptorSets({
        .sType              = VK_STRUCTURE_TYPE_BIND_DESCRIPTOR_SETS_INFO,
        .stageFlags         = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
        .layout             = m_cubemapPipeline.layout(),
        .firstSet           = 0,
        .descriptorSetCount = static_cast<uint32>(std::size(descriptorSets)),
        .pDescriptorSets    = descriptorSets,
    });
    cmd.draw(36, 1, 0, 0);
}

void Renderer::transitionAttachmentsForRender(CommandBuffer& cmd, uint32 imageIndex) {
    // Memory barrier to ensure that the image is not being read from while we are waiting on the fence
    const VkMemoryBarrier2 memoryBarrier = {
        .sType         = VK_STRUCTURE_TYPE_MEMORY_BARRIER_2,
        .pNext         = nullptr,
        .srcStageMask  = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
        .srcAccessMask = VK_ACCESS_NONE,
        .dstStageMask  = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
        .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
    };

    VkImageMemoryBarrier2 imageBarriers[3] = {
        // Transition MSAA color image to color attachment optimal
        {
            .sType               = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
            .srcStageMask        = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
            .srcAccessMask       = VK_ACCESS_NONE,
            .dstStageMask        = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            .dstAccessMask       = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
            .oldLayout           = VK_IMAGE_LAYOUT_UNDEFINED,
            .newLayout           = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
            .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .image               = m_colorImage.imageHandle(), // MSAA image
            .subresourceRange =
                {
                    .aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT,
                    .baseMipLevel   = 0,
                    .levelCount     = 1,
                    .baseArrayLayer = 0,
                    .layerCount     = 1,
                },
        },
        // Transition swapchain image to color attachment optimal
        {
            .sType               = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
            .srcStageMask        = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            .srcAccessMask       = VK_ACCESS_NONE,
            .dstStageMask        = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            .dstAccessMask       = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
            .oldLayout           = VK_IMAGE_LAYOUT_UNDEFINED,
            .newLayout           = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
            .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .image               = m_swapchain.images()[imageIndex],
            .subresourceRange =
                {
                    .aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT,
                    .baseMipLevel   = 0,
                    .levelCount     = 1,
                    .baseArrayLayer = 0,
                    .layerCount     = 1,
                },
        },
        // Transition depth image to depth stencil attachment optimal
        {
            .sType               = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
            .srcStageMask        = VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT,
            .srcAccessMask       = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
            .dstStageMask        = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
            .dstAccessMask       = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
            .oldLayout           = VK_IMAGE_LAYOUT_UNDEFINED,
            .newLayout           = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
            .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .image               = m_depthImage.imageHandle(), // Your depth image
            .subresourceRange =
                {
                    .aspectMask     = VK_IMAGE_ASPECT_DEPTH_BIT,
                    .baseMipLevel   = 0,
                    .levelCount     = 1,
                    .baseArrayLayer = 0,
                    .layerCount     = 1,
                },
        },
    };

    const VkDependencyInfo dependencyInfo = {
        .sType                    = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
        .pNext                    = nullptr,
        .dependencyFlags          = 0,
        .memoryBarrierCount       = 1,
        .pMemoryBarriers          = &memoryBarrier,
        .bufferMemoryBarrierCount = 0,
        .pBufferMemoryBarriers    = nullptr,
        .imageMemoryBarrierCount  = 3,
        .pImageMemoryBarriers     = imageBarriers,
    };
    cmd.pipelineBarrier(dependencyInfo);
}

void Renderer::transitionAttachmentsForPresent(CommandBuffer& cmd, uint32 imageIndex) {
    const VkImageMemoryBarrier2 barriers[] = {
        // Transition swapchain image for presentation
        {
            .sType               = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
            .pNext               = nullptr,
            .srcStageMask        = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            .srcAccessMask       = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
            .dstStageMask        = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
            .dstAccessMask       = VK_ACCESS_NONE, // Changed from MEMORY_READ_BIT
            .oldLayout           = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
            .newLayout           = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
            .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .image               = m_swapchain.images()[imageIndex],
            .subresourceRange    = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1},
        },
        // MSAA color image should be transitioned to a safe state too
        {
            .sType               = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
            .pNext               = nullptr,
            .srcStageMask        = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            .srcAccessMask       = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
            .dstStageMask        = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
            .dstAccessMask       = VK_ACCESS_NONE,
            .oldLayout           = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
            .newLayout           = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
            .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .image               = m_colorImage.imageHandle(),
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

    m_colorImage = Image{extent, 1, msaaSamples, ImageUsage::ColorAttachment, Format(m_swapchain.format())};
    m_depthImage = Image{extent, 1, msaaSamples, ImageUsage::DepthStencilAttachment, Format(m_ctx.queryDepthFormat())};
}

void Renderer::beginRenderingHelper(CommandBuffer& cmd, uint32 imageIndex) {
    const VkRenderingAttachmentInfo colorAttachment = {
        .sType              = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
        .pNext              = nullptr,
        .imageView          = m_colorImage.imageViewHandle(),
        .imageLayout        = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        .resolveMode        = VK_RESOLVE_MODE_AVERAGE_BIT,
        .resolveImageView   = m_swapchain.imageViews()[imageIndex],
        .resolveImageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        .loadOp             = VK_ATTACHMENT_LOAD_OP_CLEAR,
        .storeOp            = VK_ATTACHMENT_STORE_OP_STORE,
        .clearValue         = {{0.0f, 0.0f, 0.0f, 1.0f}},
    };
    const VkRenderingAttachmentInfo depthAttachment = {
        .sType              = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
        .pNext              = nullptr,
        .imageView          = m_depthImage.imageViewHandle(),
        .imageLayout        = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
        .resolveMode        = VK_RESOLVE_MODE_NONE,
        .resolveImageView   = VK_NULL_HANDLE,
        .resolveImageLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        .loadOp             = VK_ATTACHMENT_LOAD_OP_CLEAR,
        .storeOp            = VK_ATTACHMENT_STORE_OP_DONT_CARE,
        .clearValue         = {1.0f, 0},
    };
    const VkRenderingInfo renderingInfo = {
        .sType                = VK_STRUCTURE_TYPE_RENDERING_INFO,
        .pNext                = nullptr,
        .flags                = 0,
        .renderArea           = {.offset = {0, 0}, .extent = m_swapchain.extent()},
        .layerCount           = 1,
        .viewMask             = 0,
        .colorAttachmentCount = 1,
        .pColorAttachments    = &colorAttachment,
        .pDepthAttachment     = &depthAttachment,
        .pStencilAttachment   = nullptr,
    };
    cmd.beginRendering(renderingInfo);
}

void Renderer::bindPipelineHelper(CommandBuffer& cmd, const GraphicsPipeline& pipeline) {
    cmd.bindGraphicsPipeline(pipeline.pipeline());
    cmd.setScissor({.offset = {0, 0}, .extent = m_swapchain.extent()});
    cmd.setViewport({
        .x        = 0.0f,
        .y        = static_cast<float>(m_swapchain.extent().height),
        .width    = static_cast<float>(m_swapchain.extent().width),
        .height   = -static_cast<float>(m_swapchain.extent().height),
        .minDepth = 0.0f,
        .maxDepth = 1.0f,
    });
    cmd.setCullMode(VK_CULL_MODE_BACK_BIT);
    cmd.setLineWidth(1.0f);
    cmd.setFrontFace(VK_FRONT_FACE_COUNTER_CLOCKWISE);
    cmd.setDepthTestEnable(true);
}

void Renderer::writeDescriptorSetsHelper(uint32 frameIndex, uint32 numLights) {
    std::vector<VkWriteDescriptorSet> descriptorWrites;
    // MVP buffer
    const VkDescriptorBufferInfo uboBufferInfo = {
        .buffer = m_ubos[frameIndex].bufferHandle(),
        .offset = 0,
        .range  = sizeof(VertexUniformBufferObject),
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

#include "vulkan-Renderer.hpp"

#include <array>
#include <cstddef>
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
#include "core/World.hpp"
#include "engine/editor/Editor.hpp"
#include "render/Buffer.hpp"
#include "render/Flags.hpp"
#include "render/Image.hpp"
#include "render/ShaderObjects.hpp"
#include "render/Window.hpp"
#include "vulkan-Check.hpp"
#include "vulkan-CommandBuffer.hpp"
#include "vulkan-DescriptorSet.hpp"
#include "vulkan-GraphicsPipeline.hpp"
#include "vulkan-RenderContext.hpp"
#include "vulkan-Shader.hpp"
#include "vulkan-Swapchain.hpp"

namespace R3::vulkan {

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
    m_vertexShader          = Shader{m_ctx, "_spirv/pbr.vert.spv", VK_SHADER_STAGE_VERTEX_BIT};
    m_fragmentShader        = Shader{m_ctx, "_spirv/pbr.frag.spv", VK_SHADER_STAGE_FRAGMENT_BIT};
    m_cubemapVertexShader   = Shader{m_ctx, "_spirv/cubemap.vert.spv", VK_SHADER_STAGE_VERTEX_BIT};
    m_cubemapFragmentShader = Shader{m_ctx, "_spirv/cubemap.frag.spv", VK_SHADER_STAGE_FRAGMENT_BIT};
    m_shadowVertexShader    = Shader{m_ctx, "_spirv/shadow.vert.spv", VK_SHADER_STAGE_VERTEX_BIT};
    m_shadowFragmentShader  = Shader{m_ctx, "_spirv/shadow.frag.spv", VK_SHADER_STAGE_FRAGMENT_BIT};
    m_shadowGeometryShader  = Shader{m_ctx, "_spirv/shadow.geom.spv", VK_SHADER_STAGE_GEOMETRY_BIT};

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

    auto attribs = Vertex::getAttributeDescriptions();

    m_shadowPipeline = GraphicsPipeline{
        m_ctx,
        {m_shadowVertexShader, m_shadowGeometryShader},
        VK_SAMPLE_COUNT_1_BIT,
        {},
        {layout},
        {
            {
                .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
                .offset     = 0,
                .size       = sizeof(VertexPushConstantsShadow),
            },
            {
                .stageFlags = VK_SHADER_STAGE_GEOMETRY_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
                .offset     = sizeof(VertexPushConstantsShadow),
                .size       = sizeof(GeometryPushConstantsShadow),
            },
        },
        {Vertex::getBindingDescription()},
        {attribs[0]},
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
        Buffer* stagingBuffer = GResourceManager()->newFrameScopedObject<Buffer>();
        m_cubemapTexture = Cubemap{m_ctx.graphicsCommandBuffer(0), facePaths, TextureType::CubeMap, *stagingBuffer};
        m_cubemapTextureBinding = GResourceManager()->bindTexture("skybox", m_cubemapTexture);
    }
    cmd.end();
    m_ctx.submitSync(m_ctx.graphicsQueue(), cmd.commandBuffer());

    //--- Uniform Buffers
    float aspect = static_cast<float>(m_swapchain.extent().width) / static_cast<float>(m_swapchain.extent().height);

    m_viewProj = {
        .view       = glm::lookAt(fvec3(2.0f, 2.0f, 2.0f), fvec3(0.0f, 0.0f, 0.0f), fvec3(0.0f, 0.0f, 1.0f)),
        .projection = glm::perspective(glm::radians(45.0f), aspect, 0.1f, 25.0f),
    };

    m_ubos.resize(m_ctx.maxFramesInFlight());
    for (auto& ubo : m_ubos) {
        ubo = Buffer{sizeof(VertexUniformBufferObject), BufferUsage::HostUniform};
    }

    //--- Storage Buffers
    m_shadowViews.resize(m_ctx.maxFramesInFlight());
    for (auto& views : m_shadowViews) {
        views = Buffer{sizeof(fmat4) * 6, BufferUsage::HostUniform};
    }

    //--- Storage Buffers
    m_lights.resize(m_ctx.maxFramesInFlight());
    for (auto& light : m_lights) {
        light = Buffer{sizeof(PointLightShaderObject) * 256, BufferUsage::HostStorage};
    }

    //--- Shadow Maps
    cmd.begin();
    {
        Buffer* stagingBuffer = GResourceManager()->newFrameScopedObject<Buffer>();
        m_shadowMap           = Cubemap{m_ctx.graphicsCommandBuffer(0),
                              std::array<const std::byte*, 6>{},
                              1024,
                              1024,
                              4,
                              TextureType::ShadowCubeMap,
                              *stagingBuffer};
    }
    cmd.end();
    m_ctx.submitSync(m_ctx.graphicsQueue(), cmd.commandBuffer());

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
        return;
    }

    // Get current frame index
    uint32 currFrame = m_ctx.currentFrameIndex();
    m_ctx.waitForFrame(currFrame);

    // Acquire next image
    uint32 imageIndex;
    VkResult result = m_swapchain.acquireNextImage(m_ctx.imageAvailableSemaphore(currFrame), imageIndex);
    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        return; // Will be handled by resize logic
    } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        throw Exception{std::format("Failed to acquire swap chain image: {}", static_cast<int>(result))};
    }

    // update view projection matrices in ubo
    GWorld()->camera().apply(m_window.aspectRatio(), m_window.size(), m_viewProj.view, m_viewProj.projection);
    m_ubos[currFrame].copy(&m_viewProj, 0, sizeof(m_viewProj));

    uint32 numLights = updateLights(currFrame);

    fvec3 lightPos;
    GWorld()->registry().view<LightComponent>().each([&](const LightComponent& light) { lightPos = light.position; });

    auto shadowViews = getShadowViewMatrices(lightPos);
    fmat4 shadowProj = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 25.0f);
    std::array<fmat4, 6> shadowViewProj;
    for (int i = 0; i < 6; ++i) {
        shadowViewProj[i] = shadowProj * shadowViews[i];
        shadowViewProj[1][1] *= -1; // Flip Y for Vulkan
    }
    m_shadowViews[currFrame].copy(shadowViewProj.data(), 0, sizeof(fmat4) * 6);

    writeDescriptorSetsHelper(currFrame, numLights);

    CommandBuffer& cmd = m_ctx.graphicsCommandBuffer(currFrame);
    cmd.reset();

    cmd.begin();

    // renderShadowMaps(cmd, currFrame);
    // transitionShadowMapForSampling(cmd);

    transitionAttachmentsForRender(cmd, imageIndex);
    beginRenderingHelper(cmd, imageIndex);

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
    VkDescriptorSet descriptorSets[] = {m_ctx.descriptorSet(currFrame).descriptorSet()};
    cmd.bindDescriptorSets({
        .sType              = VK_STRUCTURE_TYPE_BIND_DESCRIPTOR_SETS_INFO,
        .stageFlags         = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
        .layout             = m_cubemapPipeline.layout(),
        .firstSet           = 0,
        .descriptorSetCount = static_cast<uint32>(std::size(descriptorSets)),
        .pDescriptorSets    = descriptorSets,
    });
    cmd.draw(36, 1, 0, 0);

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

            const VkBuffer vboIndices[]  = {mesh.vertexBufferIndex->handle<VkBuffer>()};
            const VkDeviceSize offsets[] = {0};
            const VkBuffer iboIndex      = mesh.indexBufferIndex->handle<VkBuffer>();
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
    submitHelper(cmd, currFrame);

    // Present - use per-image semaphore
    presentFrameHelper(currFrame, imageIndex);

    m_ctx.advanceFrame();
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
            .image               = m_colorImage.handle<VkImage>(), // MSAA image
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
            .image               = m_depthImage.handle<VkImage>(), // Your depth image
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
            .image               = m_colorImage.handle<VkImage>(),
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

void Renderer::addDescriptorMemoryBarrier(CommandBuffer& cmd) {
    // Memory barrier to ensure host writes to UBO/SSBO are visible to GPU
    const VkMemoryBarrier2 descriptorBarrier = {
        .sType         = VK_STRUCTURE_TYPE_MEMORY_BARRIER_2,
        .pNext         = nullptr,
        .srcStageMask  = VK_PIPELINE_STAGE_HOST_BIT,
        .srcAccessMask = VK_ACCESS_HOST_WRITE_BIT,
        .dstStageMask  = VK_PIPELINE_STAGE_VERTEX_SHADER_BIT | VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
        .dstAccessMask = VK_ACCESS_UNIFORM_READ_BIT | VK_ACCESS_2_SHADER_STORAGE_READ_BIT,
    };

    const VkDependencyInfo dependencyInfo = {
        .sType              = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
        .memoryBarrierCount = 1,
        .pMemoryBarriers    = &descriptorBarrier,
    };

    cmd.pipelineBarrier(dependencyInfo);
}

void Renderer::beginRenderingHelper(CommandBuffer& cmd, uint32 imageIndex) {
    const VkRenderingAttachmentInfo colorAttachment = {
        .sType              = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
        .pNext              = nullptr,
        .imageView          = m_colorImage.imageView<VkImageView>(),
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
        .imageView          = m_depthImage.imageView<VkImageView>(),
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
        .buffer = m_ubos[frameIndex].handle<VkBuffer>(),
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
        .buffer = m_lights[frameIndex].handle<VkBuffer>(),
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

    // Shadow view matrices buffer for binding 4
    const VkDescriptorBufferInfo shadowViewsBufferInfo = {
        .buffer = m_shadowViews[frameIndex].handle<VkBuffer>(),
        .offset = 0,
        .range  = sizeof(fmat4) * 6,
    };

    descriptorWrites.push_back(VkWriteDescriptorSet{
        .sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
        .dstSet          = m_ctx.descriptorSet(frameIndex).descriptorSet(),
        .dstBinding      = 4,
        .dstArrayElement = 0,
        .descriptorCount = 1,
        .descriptorType  = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        .pBufferInfo     = &shadowViewsBufferInfo,
    });

    // Shadow sampler
    const VkDescriptorImageInfo shadowImageInfo = {
        .sampler     = m_shadowMap.sampler(),
        .imageView   = m_shadowMap.imageView(),
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

void Renderer::submitHelper(CommandBuffer& cmd, uint32 frameIndex) {
    const VkSemaphoreSubmitInfo waitSemaphoreSubmitInfo = {
        .sType     = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
        .pNext     = nullptr,
        .semaphore = m_ctx.imageAvailableSemaphore(frameIndex),
        .value     = 0,
        .stageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
    };

    const VkSemaphoreSubmitInfo signalSemaphoreSubmitInfo = {
        .sType     = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
        .pNext     = nullptr,
        .semaphore = m_ctx.renderFinishedSemaphore(frameIndex),
        .value     = 0,
        .stageMask = VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT,
    };

    const VkCommandBufferSubmitInfo cmdInfo = {
        .sType         = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO,
        .pNext         = nullptr,
        .commandBuffer = cmd.commandBuffer(),
        .deviceMask    = 0,
    };

    const VkSubmitInfo2 submitInfo = {
        .sType                    = VK_STRUCTURE_TYPE_SUBMIT_INFO_2,
        .pNext                    = nullptr,
        .flags                    = {},
        .waitSemaphoreInfoCount   = 1,
        .pWaitSemaphoreInfos      = &waitSemaphoreSubmitInfo,
        .commandBufferInfoCount   = 1,
        .pCommandBufferInfos      = &cmdInfo,
        .signalSemaphoreInfoCount = 1,
        .pSignalSemaphoreInfos    = &signalSemaphoreSubmitInfo,
    };

    VK_CHECK(vkQueueSubmit2(m_ctx.graphicsQueue(), 1, &submitInfo, m_ctx.inFlightFence(frameIndex)));
}

void Renderer::presentFrameHelper(uint32 frameIndex, uint32 imageIndex) {
    const VkSemaphore signalSemaphores[] = {m_ctx.renderFinishedSemaphore(frameIndex)};
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

    VkResult result = vkQueuePresentKHR(m_ctx.graphicsQueue(), &presentInfo);

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
        // Handle swapchain recreation
    } else if (result != VK_SUCCESS) {
        throw Exception{std::format("vkQueuePresentKHR returned: {}", static_cast<int>(result))};
    }
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

std::array<fmat4, 6> Renderer::getShadowViewMatrices(const fvec3& lightPos) {
    return {{
#if 1
        glm::lookAt(lightPos, lightPos + fvec3(1, 0, 0), fvec3(0, -1, 0)),  // +X
        glm::lookAt(lightPos, lightPos + fvec3(-1, 0, 0), fvec3(0, -1, 0)), // -X
        glm::lookAt(lightPos, lightPos + fvec3(0, 1, 0), fvec3(0, 0, 1)),   // +Y
        glm::lookAt(lightPos, lightPos + fvec3(0, -1, 0), fvec3(0, 0, -1)), // -Y
        glm::lookAt(lightPos, lightPos + fvec3(0, 0, 1), fvec3(0, -1, 0)),  // +Z
        glm::lookAt(lightPos, lightPos + fvec3(0, 0, -1), fvec3(0, -1, 0)), // -Z
#else
        glm::lookAt(lightPos, lightPos + fvec3(1, 0, 0), fvec3(0, 1, 0)),  // +X
        glm::lookAt(lightPos, lightPos + fvec3(-1, 0, 0), fvec3(0, 1, 0)), // -X
        glm::lookAt(lightPos, lightPos + fvec3(0, -1, 0), fvec3(0, 0, 1)), // +Y (flipped)
        glm::lookAt(lightPos, lightPos + fvec3(0, 1, 0), fvec3(0, 0, -1)), // -Y (flipped)
        glm::lookAt(lightPos, lightPos + fvec3(0, 0, 1), fvec3(0, 1, 0)),  // +Z
        glm::lookAt(lightPos, lightPos + fvec3(0, 0, -1), fvec3(0, 1, 0)), // -Z
#endif
    }};
}

void Renderer::renderShadowMaps(CommandBuffer& cmd, uint32 frameIndex) {
    // Begin shadow rendering (you'll need to implement this)
    beginShadowRendering(cmd);

    bindPipelineHelper(cmd, m_shadowPipeline);
    cmd.setViewport({
        .x        = 0.0f,
        .y        = 1024.0f,
        .width    = 1024.0f,
        .height   = -1024.0f,
        .minDepth = 0.0f,
        .maxDepth = 1.0f,
    });
    cmd.setScissor({{0, 0}, {1024, 1024}});

    VkDescriptorSet descriptorSets[] = {m_ctx.descriptorSet(frameIndex).descriptorSet()};
    cmd.bindDescriptorSets({
        .sType              = VK_STRUCTURE_TYPE_BIND_DESCRIPTOR_SETS_INFO,
        .stageFlags         = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_GEOMETRY_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
        .layout             = m_shadowPipeline.layout(),
        .firstSet           = 0,
        .descriptorSetCount = static_cast<uint32>(std::size(descriptorSets)),
        .pDescriptorSets    = descriptorSets,
    });

    // Render all shadow casters
    GWorld()->registry().view<MeshComponent, TransformComponent>().each([&](const auto& mesh, const auto& trans) {
        VertexPushConstantsShadow vertPush   = {.model = trans.transform()};
        GeometryPushConstantsShadow geomPush = {.position = fvec3(0.0f, 2.0f, 0.0f)};

        cmd.pushConstants({
            .sType      = VK_STRUCTURE_TYPE_PUSH_CONSTANTS_INFO,
            .layout     = m_shadowPipeline.layout(),
            .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
            .size       = sizeof(VertexPushConstantsShadow),
            .pValues    = &vertPush,
        });

        cmd.pushConstants({
            .sType      = VK_STRUCTURE_TYPE_PUSH_CONSTANTS_INFO,
            .layout     = m_shadowPipeline.layout(),
            .stageFlags = VK_SHADER_STAGE_GEOMETRY_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
            .offset     = sizeof(VertexPushConstantsShadow),
            .size       = sizeof(GeometryPushConstantsShadow),
            .pValues    = &geomPush,
        });

        // Bind mesh and draw
        const VkBuffer vboIndices[]  = {mesh.vertexBufferIndex->handle<VkBuffer>()};
        const VkDeviceSize offsets[] = {0};
        const VkBuffer iboIndex      = mesh.indexBufferIndex->handle<VkBuffer>();
        cmd.bindVertexBuffers(0, vboIndices, offsets);
        cmd.bindIndexBuffer(iboIndex, 0, VK_INDEX_TYPE_UINT32);
        cmd.drawIndexed(static_cast<uint32>(mesh.indexCount));
    });

    cmd.endRendering();
}

void Renderer::beginShadowRendering(CommandBuffer& cmd) {
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
        .image               = m_shadowMap.image(), // Your shadow cubemap
        .subresourceRange =
            {
                .aspectMask     = VK_IMAGE_ASPECT_DEPTH_BIT,
                .baseMipLevel   = 0,
                .levelCount     = 1,
                .baseArrayLayer = 0,
                .layerCount     = 6, // All 6 faces
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
        .imageView          = m_shadowMap.imageView(),
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
        .layerCount           = 6,
        .viewMask             = 0,
        .colorAttachmentCount = 0, // NO color attachments
        .pColorAttachments    = nullptr,
        .pDepthAttachment     = &depthAttachment,
        .pStencilAttachment   = nullptr,
    };

    cmd.beginRendering(renderingInfo);
}

void Renderer::transitionShadowMapForSampling(CommandBuffer& cmd) {
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
        .image               = m_shadowMap.image(),
        .subresourceRange    = {VK_IMAGE_ASPECT_DEPTH_BIT, 0, 1, 0, 6},
    };

    const VkDependencyInfo dependency = {
        .sType                   = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
        .imageMemoryBarrierCount = 1,
        .pImageMemoryBarriers    = &barrier,
    };
    cmd.pipelineBarrier(dependency);
}

} // namespace R3::vulkan

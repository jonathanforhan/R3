#include "vulkan-Renderer.hpp"

#include <array>
#include <filesystem>
#include <format>
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
#include "render/Flags.hpp"
#include "render/ShaderObjects.hpp"
#include "render/Window.hpp"
#include "vulkan-Buffer.hpp"
#include "vulkan-CommandBuffer.hpp"
#include "vulkan-DescriptorSet.hpp"
#include "vulkan-GraphicsPipeline.hpp"
#include "vulkan-Image.hpp"
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
    m_colorImage     = Image{
        VkImageCreateInfo{
                .sType       = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
                .imageType   = VK_IMAGE_TYPE_2D,
                .format      = m_swapchain.format(),
                .extent      = {m_swapchain.extent().width, m_swapchain.extent().height, 1},
                .mipLevels   = 1,
                .arrayLayers = 1,
                .samples     = msaaSamples,
                .tiling      = VK_IMAGE_TILING_OPTIMAL,
                .usage       = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
        },
        VK_IMAGE_ASPECT_COLOR_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
    };
    m_depthImage = Image{
        VkImageCreateInfo{
            .sType       = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
            .imageType   = VK_IMAGE_TYPE_2D,
            .format      = m_ctx.queryDepthFormat(),
            .extent      = {m_swapchain.extent().width, m_swapchain.extent().height, 1},
            .mipLevels   = 1,
            .arrayLayers = 1,
            .samples     = msaaSamples,
            .tiling      = VK_IMAGE_TILING_OPTIMAL,
            .usage       = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
        },
        VK_IMAGE_ASPECT_DEPTH_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
    };

    //--- Shaders
    m_vertexShader          = Shader{m_ctx, "_spirv/pbr.vert.spv", VK_SHADER_STAGE_VERTEX_BIT};
    m_fragmentShader        = Shader{m_ctx, "_spirv/pbr.frag.spv", VK_SHADER_STAGE_FRAGMENT_BIT};
    m_cubemapVertexShader   = Shader{m_ctx, "_spirv/cubemap.vert.spv", VK_SHADER_STAGE_VERTEX_BIT};
    m_cubemapFragmentShader = Shader{m_ctx, "_spirv/cubemap.frag.spv", VK_SHADER_STAGE_FRAGMENT_BIT};

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

    //--- Cubemap
    CommandBuffer& cmd = m_ctx.graphicsCommandBuffer();
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
        m_cubemapTexture      = Texture{m_ctx.graphicsCommandBuffer(), facePaths, TextureType::CubeMap, *stagingBuffer};
        m_cubemapTextureBinding = GResourceManager()->bindTexture("skybox", m_cubemapTexture);
    }
    cmd.end();
    cmd.submitSync();

    //--- Uniform Buffers
    float aspect = static_cast<float>(m_swapchain.extent().width) / static_cast<float>(m_swapchain.extent().height);

    m_viewProj = {
        .view       = glm::lookAt(fvec3(2.0f, 2.0f, 2.0f), fvec3(0.0f, 0.0f, 0.0f), fvec3(0.0f, 0.0f, 1.0f)),
        .projection = glm::perspective(glm::radians(45.0f), aspect, 0.1f, 10.0f),
    };

    m_ubos.resize(m_ctx.maxFramesInFlight());
    for (auto& ubo : m_ubos) {
        ubo = Buffer{nullptr, sizeof(VertexUniformBufferObject), BufferPreset::HostUniform};
    }

    //--- Storage Buffers
    m_lights.resize(m_ctx.maxFramesInFlight());
    for (auto& light : m_lights) {
        light = Buffer{nullptr, sizeof(PointLightShaderObject) * 256, BufferPreset::HostStorage};
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
        return;
    }

    vkQueueWaitIdle(m_ctx.graphicsQueue()); // TEMP

    // Get current frame index
    // m_ctx.waitForCurrentFrame();
    uint32 currFrame = m_ctx.currentFrameIndex();

    // Acquire next image
    uint32 imageIndex;
    VkResult result = m_swapchain.acquireNextImage(m_ctx.currentImageAvailableSemaphore(), imageIndex);
    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        return; // Will be handled by resize logic
    } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        throw Exception{std::format("Failed to acquire swap chain image: {}", static_cast<int>(result))};
    }

    // update view projection matrices in ubo
    GWorld()->camera().apply(m_window.aspectRatio(), m_window.size(), m_viewProj.view, m_viewProj.projection);
    m_ubos[currFrame].copy(&m_viewProj, sizeof(m_viewProj));

    uint32 numLights = updateLights(currFrame);
    writeDescriptorSetsHelper(currFrame, numLights);

    CommandBuffer& cmd = m_ctx.graphicsCommandBuffer();
    cmd.reset();
    cmd.begin();

    transitionAttachmentsForRender(cmd, imageIndex);
    beginRenderingHelper(cmd, imageIndex);

    // cubemap
    bindPipelineHelper(cmd, m_cubemapPipeline);
    cmd.setDepthTestEnable(false); // Disable depth for skybox
    const FragmentPushConstantsCubemap fragPushConstants = {.iCubemap = m_cubemapTextureBinding};
    cmd.pushConstants(m_cubemapPipeline.layout(),
                      VK_SHADER_STAGE_FRAGMENT_BIT,
                      0,
                      sizeof(FragmentPushConstantsCubemap),
                      &fragPushConstants);
    VkDescriptorSet descriptorSets[] = {m_ctx.descriptorSet(currFrame).descriptorSet()};
    cmd.bindDescriptorSets(VK_PIPELINE_BIND_POINT_GRAPHICS, m_cubemapPipeline.layout(), 0, descriptorSets, {});
    cmd.draw(36, 1, 0, 0);

    // rest of scene
    bindPipelineHelper(cmd, m_graphicsPipeline);
    cmd.bindDescriptorSets(VK_PIPELINE_BIND_POINT_GRAPHICS, m_graphicsPipeline.layout(), 0, descriptorSets, {});

    GWorld()->registry().view<MeshComponent, MaterialComponent, TransformComponent>().each(
        [&](const MeshComponent& mesh, const MaterialComponent& mat, const TransformComponent& trans) {
            const VertexPushConstants vertPushConstants = {
                .model = trans.transform(),
            };
            cmd.pushConstants(m_graphicsPipeline.layout(),
                              VK_SHADER_STAGE_VERTEX_BIT,
                              0,
                              sizeof(VertexPushConstants),
                              &vertPushConstants);

            const FragmentPushConstants fragPushConstants = {
                .viewPosition       = GWorld()->camera().position(),
                .numLights          = numLights,
                .iAlbedo            = mat.iAlbedo,
                .iMetallicRoughness = mat.iMetallicRoughness,
                .iNormal            = mat.iNormal,
                .iAmbientOcclusion  = mat.iAmbientOcclusion,
                .iEmissive          = mat.iEmissive,
            };
            cmd.pushConstants(m_graphicsPipeline.layout(),
                              VK_SHADER_STAGE_FRAGMENT_BIT,
                              sizeof(VertexPushConstants),
                              sizeof(FragmentPushConstants),
                              &fragPushConstants);

            const VkBuffer vboIndices[]  = {mesh.vertexBufferIndex->buffer()};
            const VkDeviceSize offsets[] = {0};
            const VkBuffer iboIndex      = mesh.indexBufferIndex->buffer();
            cmd.bindVertexBuffers(0, vboIndices, offsets);
            cmd.bindIndexBuffer(iboIndex, 0, VK_INDEX_TYPE_UINT32);
            cmd.drawIndexed(static_cast<uint32>(mesh.indexCount), 1, 0, 0, 0);
        });

#if R3_EDITOR
    cmd.setDepthTestEnable(false); // Disable depth for UI
    GEngine()->m_editor->draw(cmd);
#endif

    cmd.endRendering();

    transitionAttachmentsForPresent(cmd, imageIndex);
    cmd.end();

    // Submit command buffer - use per-frame acquire, per-image render finished
    const VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    cmd.submit(m_ctx.graphicsQueue(),
               {&m_ctx.currentImageAvailableSemaphore(), 1},
               waitStages,
               {&m_ctx.renderFinishedSemaphore(imageIndex), 1},
#if 0
               m_ctx.currentFence());
#else
               VK_NULL_HANDLE);
#endif

    // Present - use per-image semaphore
    result = m_swapchain.present(m_ctx.presentQueue(), m_ctx.renderFinishedSemaphore(imageIndex), imageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
        // Will be handled by resize logic on next frame
    } else if (result != VK_SUCCESS) {
        throw Exception{std::format("Failed to present swap chain image: {}", static_cast<int>(result))};
    }

    m_ctx.advanceFrame();
}

void Renderer::transitionAttachmentsForRender(CommandBuffer& cmd, uint32 imageIndex) {
    // Transition MSAA color image to color attachment optimal
    const VkImageMemoryBarrier2 colorTransition = {
        .sType               = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
        .pNext               = nullptr,
        .srcStageMask        = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
        .srcAccessMask       = VK_ACCESS_NONE,
        .dstStageMask        = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
        .dstAccessMask       = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
        .oldLayout           = VK_IMAGE_LAYOUT_UNDEFINED,
        .newLayout           = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .image               = m_colorImage.image(), // MSAA image
        .subresourceRange =
            {
                .aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT,
                .baseMipLevel   = 0,
                .levelCount     = 1,
                .baseArrayLayer = 0,
                .layerCount     = 1,
            },
    };

    // Transition swapchain image to color attachment optimal
    const VkImageMemoryBarrier2 swapchainTransition = {
        .sType               = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
        .pNext               = nullptr,
        .srcStageMask        = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
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
    };

    // Transition depth image to depth stencil attachment optimal
    const VkImageMemoryBarrier2 depthTransition = {
        .sType               = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
        .pNext               = nullptr,
        .srcStageMask        = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
        .srcAccessMask       = VK_ACCESS_NONE,
        .dstStageMask        = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
        .dstAccessMask       = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
        .oldLayout           = VK_IMAGE_LAYOUT_UNDEFINED,
        .newLayout           = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
        .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .image               = m_depthImage.image(), // Your depth image
        .subresourceRange =
            {
                .aspectMask     = VK_IMAGE_ASPECT_DEPTH_BIT,
                .baseMipLevel   = 0,
                .levelCount     = 1,
                .baseArrayLayer = 0,
                .layerCount     = 1,
            },
    };

    // Apply all transitions
    cmd.transitionImageLayout(colorTransition);
    cmd.transitionImageLayout(swapchainTransition);
    cmd.transitionImageLayout(depthTransition);
}

void Renderer::transitionAttachmentsForPresent(CommandBuffer& cmd, uint32 imageIndex) {
    const VkImageMemoryBarrier2 presentBarrier = {
        .sType               = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
        .pNext               = nullptr,
        .srcStageMask        = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
        .srcAccessMask       = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
        .dstStageMask        = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
        .dstAccessMask       = VK_ACCESS_MEMORY_READ_BIT,
        .oldLayout           = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        .newLayout           = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
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
    };
    cmd.transitionImageLayout(presentBarrier);
}

void Renderer::handleWindowResize() {
    m_ctx.waitIdle();

    ivec2 framebufferSize = m_window.framebufferSize();
    if (framebufferSize.x == 0 || framebufferSize.y == 0) {
        return; // Minimized, skip for now
    }

    m_swapchain.recreate(m_ctx, framebufferSize);

    // recreate attachments
    auto msaaSamples = m_ctx.queryMaxUsableSampleCount();
    m_colorImage     = Image{
        VkImageCreateInfo{
                .sType       = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
                .imageType   = VK_IMAGE_TYPE_2D,
                .format      = m_swapchain.format(),
                .extent      = {m_swapchain.extent().width, m_swapchain.extent().height, 1},
                .mipLevels   = 1,
                .arrayLayers = 1,
                .samples     = msaaSamples,
                .tiling      = VK_IMAGE_TILING_OPTIMAL,
                .usage       = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
        },
        VK_IMAGE_ASPECT_COLOR_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
    };
    m_depthImage = Image{
        VkImageCreateInfo{
            .sType       = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
            .imageType   = VK_IMAGE_TYPE_2D,
            .format      = m_ctx.queryDepthFormat(),
            .extent      = {m_swapchain.extent().width, m_swapchain.extent().height, 1},
            .mipLevels   = 1,
            .arrayLayers = 1,
            .samples     = msaaSamples,
            .tiling      = VK_IMAGE_TILING_OPTIMAL,
            .usage       = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
        },
        VK_IMAGE_ASPECT_DEPTH_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
    };
}

void Renderer::beginRenderingHelper(CommandBuffer& cmd, uint32 imageIndex) {
    const VkRenderingAttachmentInfo colorAttachment = {
        .sType              = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
        .pNext              = nullptr,
        .imageView          = m_colorImage.imageView(),
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
        .imageView          = m_depthImage.imageView(),
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
        .buffer = m_ubos[frameIndex].buffer(),
        .offset = 0,
        .range  = sizeof(VertexUniformBufferObject),
    };
    descriptorWrites.push_back(VkWriteDescriptorSet{
        .sType            = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
        .pNext            = nullptr,
        .dstSet           = m_ctx.descriptorSet(frameIndex).descriptorSet(),
        .dstBinding       = 0,
        .dstArrayElement  = 0,
        .descriptorCount  = 1,
        .descriptorType   = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        .pImageInfo       = nullptr,
        .pBufferInfo      = &uboBufferInfo,
        .pTexelBufferView = nullptr,
    });

    if (numLights > 0) {
        // Storage buffer - Lights
        const VkDescriptorBufferInfo ssboBufferInfo = {
            .buffer = m_lights[frameIndex].buffer(),
            .offset = 0,
            .range  = sizeof(PointLightShaderObject) * numLights,
        };
        descriptorWrites.push_back(VkWriteDescriptorSet{
            .sType            = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
            .pNext            = nullptr,
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
        m_lights[frameIndex].copy(&lightShaderObject, sizeof(lightShaderObject), sizeof(lightShaderObject) * numLights);
        numLights++;
    });
    return numLights;
}

} // namespace R3::vulkan

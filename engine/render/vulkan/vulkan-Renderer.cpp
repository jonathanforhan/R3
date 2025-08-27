#include "vulkan-Renderer.hpp"

#include <format>
#include <span>
#include <vector>
#include <vulkan/vulkan_core.h>
#include <entt/entity/registry.hpp>
#include <entt/entity/view.hpp>
#include <entt/resource/resource.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "api/Exception.hpp"
#include "api/Types.hpp"
#include "components/MaterialComponent.hpp"
#include "components/MeshComponent.hpp"
#include "components/TransformComponent.hpp"
#include "core/Camera.hpp"
#include "core/World.hpp"
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
#include "vulkan-Texture.hpp"

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
        m_swapchain.format(),
        m_swapchain.extent(),
        1,
        msaaSamples,
        VK_IMAGE_TILING_OPTIMAL,
        VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT,
        VK_IMAGE_ASPECT_COLOR_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
    };
    m_depthImage = Image{
        m_ctx.queryDepthFormat(),
        m_swapchain.extent(),
        1,
        msaaSamples,
        VK_IMAGE_TILING_OPTIMAL,
        VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
        VK_IMAGE_ASPECT_DEPTH_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
    };

    //--- Shaders
    Shader m_vertexShader;
    Shader m_fragmentShader;
    m_vertexShader   = Shader{m_ctx, "_spirv/basic.vert.spv", VK_SHADER_STAGE_VERTEX_BIT};
    m_fragmentShader = Shader{m_ctx, "_spirv/basic.frag.spv", VK_SHADER_STAGE_FRAGMENT_BIT};

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

    //--- Graphics Pipeline
    const VkDescriptorSetLayout defaultLayout = ctx.defaultDescriptorLayout();

    const VkFormat colorFormat = m_swapchain.format();

    m_graphicsPipeline = GraphicsPipeline{
        m_ctx,
        m_vertexShader,
        m_fragmentShader,
        msaaSamples,
        std::span{&colorFormat, 1},
        std::span{&defaultLayout, 1},
    };

    World()->camera().setActive(true);
}

Renderer::~Renderer() noexcept {
    m_ctx.waitIdle();
}

void Renderer::render(double dt) {
    // Handle m_window resize
    if (m_window.shouldResize()) {
        handleWindowResize();
        m_window.setShouldResize(false);
        return;
    }

    // Skip rendering if minimized
    if (m_window.isMinimized()) {
        return;
    }

    m_ctx.waitForCurrentFrame();

    uint32 currFrame = m_ctx.currentFrameIndex();

    World()->camera().apply(m_window.aspectRatio(), m_window.size(), m_viewProj.view, m_viewProj.projection);
    m_ubos[currFrame].copy(&m_viewProj, sizeof(m_viewProj), sizeof(fmat4));

    // Acquire next image
    uint32 imageIndex;
    VkResult result = m_swapchain.acquireNextImage(m_ctx.currentImageAvailableSemaphore(), imageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        return; // Will be handled by resize logic
    } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        throw Exception{std::format("Failed to acquire swap chain image: {}", static_cast<int>(result))};
    }

    // Record command buffer
    CommandBuffer& cmd = m_ctx.graphicsCommandBuffer();
    cmd.reset();
    cmd.begin();

    transitionAttachmentsForRender(cmd, imageIndex);

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

    cmd.bindGraphicsPipeline(m_graphicsPipeline.pipeline());
    cmd.setScissor({.offset = {0, 0}, .extent = m_swapchain.extent()});
    cmd.setViewport({
        .x        = 0.0f,
        .y        = static_cast<float>(m_swapchain.extent().height), // start from bottom
        .width    = static_cast<float>(m_swapchain.extent().width),
        .height   = -static_cast<float>(m_swapchain.extent().height), // flip Y
        .minDepth = 0.0f,
        .maxDepth = 1.0f,
    });
    cmd.setCullMode(VK_CULL_MODE_BACK_BIT);
    cmd.setLineWidth(1.0f);
    cmd.setFrontFace(VK_FRONT_FACE_COUNTER_CLOCKWISE);
    cmd.setDepthTestEnable(true);

    World()->registry().view<MeshComponent, MaterialComponent, TransformComponent>().each(
        [&](const MeshComponent& mesh, MaterialComponent& mat, TransformComponent& trans) {
            std::vector<VkWriteDescriptorSet> descriptorWrites;

            trans.transform = glm::rotate(fmat4(1.0f), glm::radians(90.0f), fvec3(1.0f, 0.0f, 0.0f));
            trans.transform = glm::rotate(trans.transform, glm::radians(180.0f), fvec3(0.0f, 0.0f, 1.0f));
            m_ubos[currFrame].copy(&trans.transform, sizeof(trans.transform));

            const VkDescriptorBufferInfo bufferInfo = {
                .buffer = m_ubos[currFrame].buffer(),
                .offset = 0,
                .range  = sizeof(VertexUniformBufferObject),
            };

            descriptorWrites.push_back(VkWriteDescriptorSet{
                .sType            = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                .pNext            = nullptr,
                .dstSet           = mat.descriptorSets[currFrame].descriptorSet(),
                .dstBinding       = 0,
                .dstArrayElement  = 0,
                .descriptorCount  = 1,
                .descriptorType   = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                .pImageInfo       = nullptr,
                .pBufferInfo      = &bufferInfo,
                .pTexelBufferView = nullptr,
            });

            if (mat.albedo) {
                const VkDescriptorImageInfo imageInfo = {
                    .sampler     = mat.albedo->sampler(),
                    .imageView   = mat.albedo->imageView(),
                    .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                };

                descriptorWrites.push_back(VkWriteDescriptorSet{
                    .sType            = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                    .pNext            = nullptr,
                    .dstSet           = mat.descriptorSets[currFrame].descriptorSet(),
                    .dstBinding       = 1,
                    .dstArrayElement  = 0,
                    .descriptorCount  = 1,
                    .descriptorType   = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                    .pImageInfo       = &imageInfo,
                    .pBufferInfo      = nullptr,
                    .pTexelBufferView = nullptr,
                });
            }

            mat.descriptorSets[currFrame].write(descriptorWrites);

            VkDescriptorSet descriptorSets[] = {mat.descriptorSets[currFrame].descriptorSet()};
            cmd.bindDescriptorSets(VK_PIPELINE_BIND_POINT_GRAPHICS, m_graphicsPipeline.layout(), 0, descriptorSets, {});

            const VkBuffer vboIndices[]  = {mesh.vertexBufferIndex->buffer()};
            const VkDeviceSize offsets[] = {0};
            const VkBuffer iboIndex      = mesh.indexBufferIndex->buffer();
            cmd.bindVertexBuffers(0, vboIndices, offsets);
            cmd.bindIndexBuffer(iboIndex, 0, VK_INDEX_TYPE_UINT32);
            cmd.drawIndexed(static_cast<uint32>(mesh.indexCount), 1, 0, 0, 0);
        });

    cmd.endRendering();
    transitionAttachmentsForPresent(cmd, imageIndex);
    cmd.end();

    // Submit command buffer - use per-frame acquire, per-image render finished
    const VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    cmd.submit(m_ctx.graphicsQueue(),
               {&m_ctx.currentImageAvailableSemaphore(), 1},
               waitStages,
               {&m_ctx.renderFinishedSemaphore(currFrame), 1},
               m_ctx.currentFence());

    // Present - use per-image semaphore
    result = m_swapchain.present(m_ctx.presentQueue(), m_ctx.renderFinishedSemaphore(currFrame), imageIndex);

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
        m_swapchain.format(),
        m_swapchain.extent(),
        1,
        msaaSamples,
        VK_IMAGE_TILING_OPTIMAL,
        VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT,
        VK_IMAGE_ASPECT_COLOR_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
    };
    m_depthImage = Image{
        m_ctx.queryDepthFormat(),
        m_swapchain.extent(),
        1,
        msaaSamples,
        VK_IMAGE_TILING_OPTIMAL,
        VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
        VK_IMAGE_ASPECT_DEPTH_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
    };
}

} // namespace R3::vulkan

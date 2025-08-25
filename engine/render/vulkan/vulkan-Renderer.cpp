#include "vulkan-Renderer.hpp"

#include <format>
#include <iterator>
#include <span>
#include <vector>
#include <vulkan/vulkan_core.h>
#include <glm/gtc/matrix_transform.hpp>
#include "api/Exception.hpp"
#include "api/Types.hpp"
#include "components/MeshComponent.hpp"
#include "core/Camera.hpp"
#include "core/World.hpp"
#include "render/Flags.hpp"
#include "render/ShaderObjects.hpp"
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
        m_ctx,
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
        m_ctx,
        m_ctx.queryDepthFormat(),
        m_swapchain.extent(),
        1,
        msaaSamples,
        VK_IMAGE_TILING_OPTIMAL,
        VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
        VK_IMAGE_ASPECT_DEPTH_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
    };

    //--- Render Pass with colorAttachment and depthAttachment
    m_renderPass = RenderPassBuilder()
                       .addMSAAColorAttachment(m_swapchain.format(), msaaSamples)
                       .setDepthStencilAttachment(m_ctx.queryDepthFormat(), msaaSamples)
                       .build(m_ctx);

    //--- Shaders
    m_vertexShader   = Shader{m_ctx, "_spirv/basic.vert.spv", VK_SHADER_STAGE_VERTEX_BIT};
    m_fragmentShader = Shader{m_ctx, "_spirv/basic.frag.spv", VK_SHADER_STAGE_FRAGMENT_BIT};

    //--- Texture
    CommandBuffer& cmd = m_ctx.graphicsCommandBuffer(0);
    cmd.reset();
    cmd.begin();
    m_texture = Texture{m_ctx, cmd, "textures/statue_head.jpg", TextureType::Albedo};
    cmd.end();
    cmd.submit(m_ctx.graphicsQueue());

    //--- Uniform Buffers
    float aspect = static_cast<float>(m_swapchain.extent().width) / static_cast<float>(m_swapchain.extent().height);

    m_ubo = {
        .model = fmat4(1.0f),
        .view  = glm::lookAt(fvec3(2.0f, 2.0f, 2.0f), fvec3(0.0f, 0.0f, 0.0f), fvec3(0.0f, 0.0f, 1.0f)),
        .proj  = glm::perspective(glm::radians(45.0f), aspect, 0.1f, 10.0f),
    };

    m_ubos.resize(m_ctx.maxFramesInFlight());
    for (auto& ubo : m_ubos) {
        const VkMemoryPropertyFlags bufferMemoryFlags =
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
        ubo = Buffer{m_ctx, sizeof(UniformBufferObject), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, bufferMemoryFlags};
    }

    //--- Descriptor Pool
    VkDescriptorPoolSize poolSizes[] = {
        {.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, .descriptorCount = m_ctx.maxFramesInFlight()},
        {.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, .descriptorCount = m_ctx.maxFramesInFlight()},
        {.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, .descriptorCount = m_ctx.maxFramesInFlight()},
        {.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, .descriptorCount = m_ctx.maxFramesInFlight()},
        {.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, .descriptorCount = m_ctx.maxFramesInFlight()},
        {.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, .descriptorCount = m_ctx.maxFramesInFlight()},
    };
    m_descriptorAllocator.create(m_ctx, poolSizes, m_ctx.maxFramesInFlight());

    //--- Descriptor Sets and Layouts
    VkDescriptorSetLayoutBinding bindings[] = {
        // { binding, type, count, stage }

        // Uniform Buffer Object
        {0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT},
        // Albedo
        {1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT},
        // MetallicRoughness
        {2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT},
        // Normal
        {3, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT},
        // AmbientOcclusion
        {4, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT},
        // Emissive
        {5, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT},
    };
    m_descriptorSets = m_descriptorAllocator.allocate(bindings, m_ctx.maxFramesInFlight());

    //--- Graphics Pipeline
    auto layout{m_descriptorAllocator.layout()};
    m_graphicsPipeline = GraphicsPipeline{
        m_ctx,
        m_renderPass,
        m_vertexShader,
        m_fragmentShader,
        msaaSamples,
        std::span{&layout, 1},
    };

    for (uint32 i = 0; i < m_ctx.maxFramesInFlight(); i++) {
        const VkDescriptorBufferInfo bufferInfo = {
            .buffer = m_ubos[i].buffer(),
            .offset = 0,
            .range  = sizeof(UniformBufferObject),
        };

        const VkDescriptorImageInfo imageInfo = {
            .sampler     = m_texture.sampler(),
            .imageView   = m_texture.imageView(),
            .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
        };

        const VkWriteDescriptorSet descriptorWrites[] = {
            {
                .sType            = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                .pNext            = nullptr,
                .dstSet           = m_descriptorSets[i],
                .dstBinding       = 0,
                .dstArrayElement  = 0,
                .descriptorCount  = 1,
                .descriptorType   = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                .pImageInfo       = nullptr,
                .pBufferInfo      = &bufferInfo,
                .pTexelBufferView = nullptr,
            },
            {
                .sType            = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                .pNext            = nullptr,
                .dstSet           = m_descriptorSets[i],
                .dstBinding       = 1,
                .dstArrayElement  = 0,
                .descriptorCount  = 1,
                .descriptorType   = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                .pImageInfo       = &imageInfo,
                .pBufferInfo      = nullptr,
                .pTexelBufferView = nullptr,
            },
        };
        vkUpdateDescriptorSets(m_ctx.device(), (uint32)std::size(descriptorWrites), descriptorWrites, 0, nullptr);
    }

    //--- Framebuffers
    for (size_t i = 0; i < m_swapchain.imageViews().size(); i++) {
        const VkImageView attachments[] = {
            m_colorImage.imageView(),
            m_swapchain.imageViews()[i],
            m_depthImage.imageView(),
        };
        m_framebuffers.emplace_back(m_ctx, m_renderPass, attachments, m_swapchain.extent());
    }

    //--- Frame Sync
    m_frameSync = FrameSync{m_ctx, m_ctx.maxFramesInFlight(), m_swapchain.images().size()};

    World()->camera().setActive(true);
}

Renderer::~Renderer() noexcept {
    m_ctx.waitIdle();
    m_descriptorAllocator.destroy();
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

    m_frameSync.waitForCurrentFrame();
    m_frameSync.resetCurrentFrame();

    World()->camera().apply(m_window.aspectRatio(), m_window.size(), m_ubo.view, m_ubo.proj);
    m_ubos[m_frameSync.currentFrameIndex()].copy(&m_ubo, sizeof(m_ubo));

    // Acquire next image
    uint32 imageIndex;
    VkResult result = m_swapchain.acquireNextImage(m_frameSync.currentImageAvailableSemaphore(), imageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        return; // Will be handled by resize logic
    } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        throw Exception{std::format("Failed to acquire swap chain image: {}", static_cast<int>(result))};
    }

    // Record command buffer
    CommandBuffer& cmd = m_ctx.graphicsCommandBuffer(m_frameSync.currentFrameIndex());
    cmd.reset();
    cmd.begin();

    // Begin render pass
    const VkClearValue clearValues[] = {
        {.color = {{0.0f, 0.0f, 0.0f, 1.0f}}},
        {.color = {{0.0f, 0.0f, 0.0f, 1.0f}}},
        {.depthStencil = {1.0f, 0}},
    };
    const VkRenderPassBeginInfo renderPassInfo = {
        .sType       = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
        .pNext       = nullptr,
        .renderPass  = m_renderPass.renderPass(),
        .framebuffer = m_framebuffers[imageIndex].framebuffer(),
        .renderArea =
            {
                .offset = {0, 0},
                .extent = m_swapchain.extent(),
            },
        .clearValueCount = static_cast<uint32>(std::size(clearValues)),
        .pClearValues    = clearValues,
    };
    cmd.beginRenderPass(renderPassInfo);
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

    cmd.bindDescriptorSets(VK_PIPELINE_BIND_POINT_GRAPHICS,
                           m_graphicsPipeline.layout(),
                           0,
                           {&m_descriptorSets[m_frameSync.currentFrameIndex()], 1},
                           {});

    World()->registry().view<MeshComponent>().each([&](const MeshComponent& mesh) {
        const usize vboIndices[]     = {mesh.vertexBufferIndex};
        const VkDeviceSize offsets[] = {0};
        const usize iboIndex         = mesh.indexBufferIndex;
        cmd.bindVertexBuffers(0, vboIndices, offsets);
        cmd.bindIndexBuffer(iboIndex, 0, VK_INDEX_TYPE_UINT32);
        cmd.drawIndexed(static_cast<uint32>(mesh.indexCount), 1, 0, 0, 0);
    });

    cmd.endRenderPass();
    cmd.end();

    // Submit command buffer - use per-frame acquire, per-image render finished
    const VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    cmd.submit(m_ctx.graphicsQueue(),
               {&m_frameSync.currentImageAvailableSemaphore(), 1},
               waitStages,
               {&m_frameSync.renderFinishedSemaphore(imageIndex), 1},
               m_frameSync.currentFence());

    // Present - use per-image semaphore
    result = m_swapchain.present(m_ctx.presentQueue(), m_frameSync.renderFinishedSemaphore(imageIndex), imageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
        // Will be handled by resize logic on next frame
    } else if (result != VK_SUCCESS) {
        throw Exception{std::format("Failed to present swap chain image: {}", static_cast<int>(result))};
    }

    m_frameSync.advanceFrame();
}

void Renderer::handleWindowResize() {
    m_ctx.waitIdle();

    ivec2 framebufferSize = m_window.framebufferSize();

    if (framebufferSize.x == 0 || framebufferSize.y == 0) {
        return; // Minimized, skip for now
    }

    m_swapchain.recreate(m_ctx, framebufferSize);

    auto msaaSamples = m_ctx.queryMaxUsableSampleCount();
    m_colorImage     = Image{
        m_ctx,
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
        m_ctx,
        m_ctx.queryDepthFormat(),
        m_swapchain.extent(),
        1,
        msaaSamples,
        VK_IMAGE_TILING_OPTIMAL,
        VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
        VK_IMAGE_ASPECT_DEPTH_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
    };

    m_framebuffers.clear();

    for (size_t i = 0; i < m_swapchain.imageViews().size(); i++) {
        const VkImageView attachments[] = {
            m_colorImage.imageView(),
            m_swapchain.imageViews()[i],
            m_depthImage.imageView(),
        };
        m_framebuffers.emplace_back(m_ctx, m_renderPass, attachments, m_swapchain.extent());
    }

    m_frameSync.recreateImageSync(m_swapchain.images().size());
}

} // namespace R3::vulkan

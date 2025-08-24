#include "vulkan-Renderer.hpp"

#include <format>
#include <iterator>
#include <span>
#include <vector>
#include <vulkan/vulkan_core.h>
#include <glm/gtc/matrix_transform.hpp>
#include "api/Exception.hpp"
#include "api/Types.hpp"
#include "core/Camera.hpp"
#include "core/World.hpp"
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

static constexpr uint32 MAX_FRAMES_IN_FLIGHT = 3;

// Triangle vertices - matches your vertex shader (vec3 position, vec3 color)
static const Vertex s_vertices[] = {
    {{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
    {{0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
    {{0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
    {{-0.5f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}},

    {{-0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
    {{0.5f, -0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
    {{0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
    {{-0.5f, 0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}},
};

static const uint16 s_indices[] = {0, 1, 2, 2, 3, 0, 4, 5, 6, 6, 7, 4};

Renderer::Renderer(Window& window)
    : m_window(window) {
    //--- Render Context
    //    - instance
    //    - surface
    //    - physical device
    //    - logical device
    m_ctx = RenderContext{m_window};

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

    //--- Command Buffers
    //    - Each collection shares a VkCommandPool
    const VkCommandPoolCreateFlags poolFlags =
        VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT | VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
    m_graphicsQueueCmds = CommandBuffer::allocate(m_ctx, m_ctx.graphicsQueueIndex(), poolFlags, MAX_FRAMES_IN_FLIGHT);
    m_computeQueueCmds  = CommandBuffer::allocate(m_ctx, m_ctx.computeQueueIndex(), poolFlags, MAX_FRAMES_IN_FLIGHT);

    //--- Shaders
    m_vertexShader   = Shader{m_ctx, "_spirv/basic.vert.spv", VK_SHADER_STAGE_VERTEX_BIT};
    m_fragmentShader = Shader{m_ctx, "_spirv/basic.frag.spv", VK_SHADER_STAGE_FRAGMENT_BIT};

    //--- Vertex/Index Buffers
    const VkMemoryPropertyFlags bufferMemoryFlags =
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
    m_vertexBuffer = Buffer{m_ctx, sizeof(s_vertices), VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, bufferMemoryFlags};
    m_vertexBuffer.copy(&s_vertices, sizeof(s_vertices));
    m_indexBuffer = Buffer{m_ctx, sizeof(s_indices), VK_BUFFER_USAGE_INDEX_BUFFER_BIT, bufferMemoryFlags};
    m_indexBuffer.copy(&s_indices, sizeof(s_indices));

    //--- Texture
    CommandBuffer& cmd = m_graphicsQueueCmds[0];
    cmd.reset();
    cmd.begin();
    m_texture = Texture{m_ctx, cmd, "textures/statue_head.jpg", TextureType::Albedo};
    cmd.end();
    cmd.submit(m_ctx.graphicsQueue());

    //--- Uniform Buffers
    m_ubo = {
        .model = fmat4(1.0f),
        .view  = glm::lookAt(fvec3(2.0f, 2.0f, 2.0f), fvec3(0.0f, 0.0f, 0.0f), fvec3(0.0f, 0.0f, 1.0f)),
        .proj  = glm::perspective(
            glm::radians(45.0f),
            static_cast<float>(m_swapchain.extent().width) / static_cast<float>(m_swapchain.extent().height),
            0.1f,
            10.0f),
    };

    m_ubos.resize(MAX_FRAMES_IN_FLIGHT);
    for (auto& ubo : m_ubos) {
        ubo = Buffer{m_ctx, sizeof(UniformBufferObject), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, bufferMemoryFlags};
    }

    //--- Descriptor Pool
    VkDescriptorPoolSize poolSizes[] = {
        {.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, .descriptorCount = MAX_FRAMES_IN_FLIGHT},
        {.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, .descriptorCount = MAX_FRAMES_IN_FLIGHT},
    };
    m_descriptorAllocator.create(m_ctx, poolSizes, MAX_FRAMES_IN_FLIGHT);

    //--- Descriptor Sets and Layouts
    VkDescriptorSetLayoutBinding bindings[] = {
        // vertices
        {
            .binding            = 0,
            .descriptorType     = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            .descriptorCount    = 1,
            .stageFlags         = VK_SHADER_STAGE_VERTEX_BIT,
            .pImmutableSamplers = nullptr,
        },
        // sampler
        {
            .binding            = 1,
            .descriptorType     = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
            .descriptorCount    = 1,
            .stageFlags         = VK_SHADER_STAGE_FRAGMENT_BIT,
            .pImmutableSamplers = nullptr,
        },
    };
    m_descriptorSets = m_descriptorAllocator.allocate(bindings, MAX_FRAMES_IN_FLIGHT);

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

    for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
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
    m_frameSync = FrameSync{m_ctx, MAX_FRAMES_IN_FLIGHT, m_swapchain.images().size()};

    World::instance().camera().setActive(true);
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

    World::instance().camera().apply(m_window.aspectRatio(), m_window.size(), m_ubo.view, m_ubo.proj);
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
    CommandBuffer& cmd = m_graphicsQueueCmds[m_frameSync.currentFrameIndex()];
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
        .y        = 0.0f,
        .width    = static_cast<float>(m_swapchain.extent().width),
        .height   = static_cast<float>(m_swapchain.extent().height),
        .minDepth = 0.0f,
        .maxDepth = 1.0f,
    });
    cmd.setCullMode(VK_CULL_MODE_NONE); // TODO revert back to VK_CULL_MODE_BACK_BIT
    cmd.setLineWidth(1.0f);
    cmd.setFrontFace(VK_FRONT_FACE_COUNTER_CLOCKWISE);
    cmd.setDepthTestEnable(true);

    cmd.bindDescriptorSets(VK_PIPELINE_BIND_POINT_GRAPHICS,
                           m_graphicsPipeline.layout(),
                           0,
                           {&m_descriptorSets[m_frameSync.currentFrameIndex()], 1},
                           {});
    const VkBuffer vertexBuffers[] = {m_vertexBuffer.buffer()};
    const VkDeviceSize offsets[]   = {0};
    cmd.bindVertexBuffers(0, vertexBuffers, offsets);
    cmd.bindIndexBuffer(m_indexBuffer.buffer(), 0, VK_INDEX_TYPE_UINT16);
    cmd.drawIndexed(static_cast<uint32>(std::size(s_indices)), 1, 0, 0, 0);

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

    m_frameSync.recreateImageSync(m_ctx, m_swapchain.images().size());
}

} // namespace R3::vulkan

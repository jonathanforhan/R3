#if R3_VULKAN

#include "vulkan-Renderer.hpp"

#include <format>
#include <iterator>
#include <span>
#include <vector>
#include <vulkan/vulkan_core.h>
#include <glm/gtc/matrix_transform.hpp>
#include "Camera.hpp"
#include "Exception.hpp"
#include "Types.hpp"
#include "render/Window.hpp"
#include "vulkan-Buffer.hpp"
#include "vulkan-Check.hpp"
#include "vulkan-CommandAllocator.hpp"
#include "vulkan-DescriptorAllocator.hpp"
#include "vulkan-FrameSync.hpp"
#include "vulkan-Framebuffer.hpp"
#include "vulkan-GraphicsPipeline.hpp"
#include "vulkan-RenderContext.hpp"
#include "vulkan-RenderPass.hpp"
#include "vulkan-Shader.hpp"
#include "vulkan-Swapchain.hpp"
#include "vulkan-Texture.hpp"

namespace R3::vulkan {

using namespace R3::vulkan;

static constexpr uint32 MAX_FRAMES_IN_FLIGHT = 3;

// Triangle vertices - matches your vertex shader (vec3 position, vec3 color)
static const Vertex s_vertices[] = {
    {{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
    {{0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
    {{0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
    {{-0.5f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}},
};

static const uint16_t s_indices[] = {0, 1, 2, 2, 3, 0};

Renderer::Renderer(Window& window)
    : m_window{window} {
    //--- Render Context
    //    - instance
    //    - surface
    //    - physical device
    //    - logical device
    m_ctx.create(m_window);

    //--- Swapchain
    m_swapchain.create(m_window, m_ctx);

    //--- Render Pass with colorAttachment for subpass
    const VkAttachmentDescription colorAttachment = {
        .flags          = {},
        .format         = m_swapchain.format(),
        .samples        = VK_SAMPLE_COUNT_1_BIT,
        .loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR,
        .storeOp        = VK_ATTACHMENT_STORE_OP_STORE,
        .stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
        .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
        .initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED,
        .finalLayout    = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
    };
    m_renderPass.create(m_ctx, std::span{&colorAttachment, 1});

    //--- Framebuffers
    m_framebuffers.resize(m_swapchain.imageViews().size());
    for (size_t i = 0; i < m_swapchain.imageViews().size(); i++) {
        const VkImageView attachments[] = {m_swapchain.imageViews()[i]};
        m_framebuffers[i].create(m_ctx, m_renderPass, attachments, m_swapchain.extent());
    }

    //--- Command Buffers
    m_commandAllocator.create(m_ctx, m_ctx.graphicsQueueIndex(), VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
    m_commandBuffers = m_commandAllocator.allocateBuffers(MAX_FRAMES_IN_FLIGHT);

    //--- Shaders
    m_vertexShader.createFromFile(m_ctx, "_spirv/basic.vert.spv", VK_SHADER_STAGE_VERTEX_BIT);
    m_fragmentShader.createFromFile(m_ctx, "_spirv/basic.frag.spv", VK_SHADER_STAGE_FRAGMENT_BIT);

    //--- Descriptor Pool
    VkDescriptorPoolSize poolSizes[] = {
        {
            .type            = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            .descriptorCount = MAX_FRAMES_IN_FLIGHT,
        },
        {
            .type            = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
            .descriptorCount = MAX_FRAMES_IN_FLIGHT,
        },
    };
    m_descriptorAllocator.create(m_ctx, poolSizes, MAX_FRAMES_IN_FLIGHT);

    //--- Vertex Buffer
    m_vertexBuffer.create(m_ctx,
                          sizeof(s_vertices[0]) * std::size(s_vertices),
                          VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                          VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    m_vertexBuffer.copy(&s_vertices, sizeof(s_vertices));

    //--- Index Buffer
    m_indexBuffer.create(m_ctx,
                         sizeof(s_indices[0]) * std::size(s_indices),
                         VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                         VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    m_indexBuffer.copy(&s_indices, sizeof(s_indices));

    //--- Texture
    m_texture.create(m_ctx, m_commandAllocator.allocateBuffer(), "textures/statue_head.jpg", TextureType::Albedo);

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
        ubo.create(m_ctx,
                   sizeof(UniformBufferObject),
                   VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                   VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    }

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
    auto layout = m_descriptorAllocator.layout();
    m_graphicsPipeline.create(m_ctx, m_renderPass, m_vertexShader, m_fragmentShader, std::span{&layout, 1});

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

    //--- Frame Sync
    const size_t imageCount = m_swapchain.images().size();
    m_frameSync.create(m_ctx, MAX_FRAMES_IN_FLIGHT, imageCount);

    m_camera.setActive(true);
}

Renderer::~Renderer() noexcept {
    m_ctx.waitIdle();

    m_frameSync.destroy();
    m_graphicsPipeline.destroy();
    m_descriptorAllocator.destroy();
    for (auto& ubo : m_ubos) {
        ubo.destroy();
    }
    m_texture.destroy();
    m_indexBuffer.destroy();
    m_vertexShader.destroy();
    m_fragmentShader.destroy();
    m_vertexBuffer.destroy();
    m_commandAllocator.destroy();
    for (auto& framebuffer : m_framebuffers) {
        framebuffer.destroy();
    }
    m_renderPass.destroy();
    m_swapchain.destroy();
    m_ctx.destroy();
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

    m_camera.tick(dt);
    m_camera.apply(m_window.aspectRatio(), m_window.size(), m_ubo.view, m_ubo.proj);
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
    const VkCommandBuffer cmd = m_commandBuffers[m_frameSync.currentFrameIndex()];
    vkResetCommandBuffer(cmd, 0);

    const VkCommandBufferBeginInfo beginInfo = {
        .sType            = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .pNext            = nullptr,
        .flags            = {},
        .pInheritanceInfo = nullptr,
    };
    VK_CHECK(vkBeginCommandBuffer(cmd, &beginInfo));

    // Begin render pass
    const VkClearValue clearValue{{{0.0f, 0.0f, 0.0f, 1.0f}}};
    const VkRenderPassBeginInfo renderPassInfo{
        .sType       = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
        .pNext       = nullptr,
        .renderPass  = m_renderPass.renderPass(),
        .framebuffer = m_framebuffers[imageIndex].framebuffer(),
        .renderArea =
            {
                .offset = {0, 0},
                .extent = m_swapchain.extent(),
            },
        .clearValueCount = 1,
        .pClearValues    = &clearValue,
    };

    vkCmdBeginRenderPass(cmd, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    // Bind pipeline and draw
    m_graphicsPipeline.bind(cmd);

    const VkViewport viewport = {
        .x        = 0.0f,
        .y        = 0.0f,
        .width    = static_cast<float>(m_swapchain.extent().width),
        .height   = static_cast<float>(m_swapchain.extent().height),
        .minDepth = 0.0f,
        .maxDepth = 1.0f,
    };

    const VkRect2D scissor = {
        .offset = {0, 0},
        .extent = m_swapchain.extent(),
    };

    m_graphicsPipeline.setScissor(cmd, scissor);
    m_graphicsPipeline.setViewport(cmd, viewport);
    m_graphicsPipeline.setCullMode(cmd, VK_CULL_MODE_BACK_BIT);
    m_graphicsPipeline.setFrontFace(cmd, VK_FRONT_FACE_COUNTER_CLOCKWISE);
    m_graphicsPipeline.setLineWidth(cmd, 1.0f);

    vkCmdBindDescriptorSets(cmd,
                            VK_PIPELINE_BIND_POINT_GRAPHICS,
                            m_graphicsPipeline.layout(),
                            0,
                            1,
                            &m_descriptorSets[m_frameSync.currentFrameIndex()],
                            0,
                            nullptr);

    const VkBuffer vertexBuffers[] = {m_vertexBuffer.buffer()};
    const VkDeviceSize offsets[]   = {0};
    vkCmdBindVertexBuffers(cmd, 0, 1, vertexBuffers, offsets);
    vkCmdBindIndexBuffer(cmd, m_indexBuffer.buffer(), 0, VK_INDEX_TYPE_UINT16);
    vkCmdDrawIndexed(cmd, static_cast<uint32>(std::size(s_indices)), 1, 0, 0, 0);

    vkCmdEndRenderPass(cmd);
    VK_CHECK(vkEndCommandBuffer(cmd));

    // Submit command buffer - use per-frame acquire, per-image render finished
    const VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    const VkSubmitInfo submitInfo{
        .sType                = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .pNext                = nullptr,
        .waitSemaphoreCount   = 1,
        .pWaitSemaphores      = &m_frameSync.currentImageAvailableSemaphore(),
        .pWaitDstStageMask    = waitStages,
        .commandBufferCount   = 1,
        .pCommandBuffers      = &cmd,
        .signalSemaphoreCount = 1,
        .pSignalSemaphores    = &m_frameSync.renderFinishedSemaphore(imageIndex),
    };
    VK_CHECK(vkQueueSubmit(m_ctx.graphicsQueue(), 1, &submitInfo, m_frameSync.currentFence()));

    // Present - use per-image semaphore
    result = m_swapchain.present(m_ctx.presentQueue(), m_frameSync.renderFinishedSemaphore(imageIndex), imageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
        // Will be handled by resize logic on next frame
    } else if (result != VK_SUCCESS) {
        throw Exception{std::format("Failed to present swap chain image: {}", static_cast<int>(result))};
    }

    m_frameSync.advanceFrame();
} // namespace R3::vulkan

void Renderer::handleWindowResize() {
    m_ctx.waitIdle();

    for (auto& framebuffer : m_framebuffers) {
        framebuffer.destroy();
    }

    m_swapchain.recreate(m_window, m_ctx);
    m_frameSync.recreateImageSync(m_ctx, m_swapchain.images().size());

    m_framebuffers.resize(m_swapchain.imageViews().size());
    for (size_t i = 0; i < m_swapchain.imageViews().size(); i++) {
        const VkImageView attachments[] = {m_swapchain.imageViews()[i]};
        m_framebuffers[i].create(m_ctx, m_renderPass, attachments, m_swapchain.extent());
    }
}

} // namespace R3::vulkan

#endif // R3_VULKAN

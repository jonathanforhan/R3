#include "Renderer.hpp"

#include <format>
#include <iterator>
#include <span>
#include <vector>
#include <vulkan/vulkan_core.h>
#include "Buffer.hpp"
#include "CommandAllocator.hpp"
#include "Exception.hpp"
#include "FrameSync.hpp"
#include "Framebuffer.hpp"
#include "GraphicsPipeline.hpp"
#include "Log.hpp"
#include "RenderContext.hpp"
#include "RenderPass.hpp"
#include "Shader.hpp"
#include "Swapchain.hpp"
#include "Types.hpp"
#include "Window.hpp"

namespace R3 {

#define VK_CHECK(_Exp)                                    \
    if (VkResult _result = (_Exp); _result != VK_SUCCESS) \
    throw ::R3::Exception(std::format(#_Exp " returned: ", static_cast<int>(_result)))

using namespace R3;

static constexpr uint32 MAX_FRAMES_IN_FLIGHT = 3;

// Triangle vertices - matches your vertex shader (vec3 position, vec3 color)
static const Vertex s_vertices[3] = {
    {{0.0f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}}, // Top - Red
    {{-0.5f, 0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}}, // Bottom left - Green
    {{0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}},  // Bottom right - Blue
};

Renderer::Renderer(Window& window)
    : m_window{window} {
    m_ctx.create(m_window);
    m_swapchain.create(m_ctx, m_window);
    const AttachmentDescription colorAttachment = {
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
    m_vertexShader.createFromFile(m_ctx, "_spirv/basic.vert.spv", ShaderStageFlags::Vertex);
    m_fragmentShader.createFromFile(m_ctx, "_spirv/basic.frag.spv", ShaderStageFlags::Fragment);
    m_graphicsPipeline.create(m_ctx, m_renderPass, m_vertexShader, m_fragmentShader, m_swapchain.extent());
    m_vertexBuffer.create(m_ctx,
                          sizeof(s_vertices[0]) * std::size(s_vertices),
                          BufferUsageFlags::VertexBuffer,
                          MemoryPropertyFlags::HostVisible | MemoryPropertyFlags::HostCoherent);
    m_vertexBuffer.copyData(s_vertices);

    m_framebuffers.resize(m_swapchain.imageViews().size());
    for (size_t i = 0; i < m_swapchain.imageViews().size(); i++) {
        const VkImageView attachments[] = {m_swapchain.imageViews()[i]};
        m_framebuffers[i].create(m_ctx, m_renderPass, attachments, m_swapchain.extent());
    }

    m_commandAllocator.create(m_ctx, m_ctx.graphicsQueue().index, CommandPoolModeFlags::Reset);
    m_commandBuffers = m_commandAllocator.allocateBuffers(MAX_FRAMES_IN_FLIGHT);

    const size_t imageCount = m_swapchain.images().size();
    m_frameSync.create(m_ctx, MAX_FRAMES_IN_FLIGHT, imageCount);
}

Renderer::~Renderer() noexcept {
    // Wait for device to finish
    m_ctx.waitIdle();

    // Cleanup
    m_frameSync.destroy();
    for (auto& framebuffer : m_framebuffers) {
        framebuffer.destroy();
    }
    m_commandAllocator.destroy();
    m_vertexBuffer.destroy();
    m_graphicsPipeline.destroy();
    m_fragmentShader.destroy();
    m_vertexShader.destroy();
    m_renderPass.destroy();
    m_swapchain.destroy();
    m_ctx.destroy();
}

void Renderer::render() {
    // Main render loop
    while (!m_window.shouldClose()) {
        m_window.update();

        // Handle m_window resize
        if (m_window.shouldResize()) {
            m_ctx.waitIdle();

            // Destroy old framebuffers
            for (auto& framebuffer : m_framebuffers) {
                framebuffer.destroy();
            }

            m_swapchain.recreate(m_ctx, m_window);
            m_frameSync.recreateImageSync(m_ctx, m_swapchain.images().size());

            // Recreate framebuffers
            m_framebuffers.resize(m_swapchain.imageViews().size());
            for (size_t i = 0; i < m_swapchain.imageViews().size(); i++) {
                const VkImageView attachments[] = {m_swapchain.imageViews()[i]};
                m_framebuffers[i].create(m_ctx, m_renderPass, attachments, m_swapchain.extent());
            }

            m_window.setShouldResize(false);
            continue;
        }

        // Skip rendering if minimized
        if (m_window.isMinimized()) {
            continue;
        }

        m_frameSync.waitForCurrentFrame();
        m_frameSync.resetCurrentFrame();

        // Acquire next image
        uint32_t imageIndex;
        VkResult result = m_swapchain.acquireNextImage(m_frameSync.currentImageAvailableSemaphore(), imageIndex);

        if (result == VK_ERROR_OUT_OF_DATE_KHR) {
            continue; // Will be handled by resize logic
        } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
            throw Exception{std::format("Failed to acquire swap chain image: {}", static_cast<int>(result))};
        }

        // Record command buffer
        const VkCommandBuffer commandBuffer = m_commandBuffers[m_frameSync.currentFrameIndex()];
        vkResetCommandBuffer(commandBuffer, 0);

        const VkCommandBufferBeginInfo beginInfo = {
            .sType            = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
            .pNext            = nullptr,
            .flags            = {},
            .pInheritanceInfo = nullptr,
        };
        VK_CHECK(vkBeginCommandBuffer(commandBuffer, &beginInfo));

        // Begin render pass
        const VkClearValue clearValue{{{1.0f, 0.0f, 1.0f, 1.0f}}};
        const VkRenderPassBeginInfo renderPassInfo{
            .sType       = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
            .pNext       = nullptr,
            .renderPass  = m_renderPass.handle(),
            .framebuffer = m_framebuffers[imageIndex].handle(),
            .renderArea =
                {
                    .offset = {0, 0},
                    .extent = {m_swapchain.extent().x, m_swapchain.extent().y},
                },
            .clearValueCount = 1,
            .pClearValues    = &clearValue,
        };

        vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        // Bind pipeline and draw
        m_graphicsPipeline.bind(commandBuffer);

        const VkBuffer vertexBuffers[] = {m_vertexBuffer.handle()};
        const VkDeviceSize offsets[]   = {0};
        vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);

        vkCmdDraw(commandBuffer, static_cast<uint32_t>(std::size(s_vertices)), 1, 0, 0);

        vkCmdEndRenderPass(commandBuffer);
        VK_CHECK(vkEndCommandBuffer(commandBuffer));

        // Submit command buffer - use per-frame acquire, per-image render finished
        const VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
        const VkSubmitInfo submitInfo{
            .sType                = VK_STRUCTURE_TYPE_SUBMIT_INFO,
            .pNext                = nullptr,
            .waitSemaphoreCount   = 1,
            .pWaitSemaphores      = &m_frameSync.currentImageAvailableSemaphore(),
            .pWaitDstStageMask    = waitStages,
            .commandBufferCount   = 1,
            .pCommandBuffers      = &commandBuffer,
            .signalSemaphoreCount = 1,
            .pSignalSemaphores    = &m_frameSync.renderFinishedSemaphore(imageIndex),
        };
        VK_CHECK(vkQueueSubmit(m_ctx.graphicsQueue().handle, 1, &submitInfo, m_frameSync.currentFence()));

        // Present - use per-image semaphore
        result = m_swapchain.present(
            m_ctx.presentQueue().handle, m_frameSync.renderFinishedSemaphore(imageIndex), imageIndex);

        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
            // Will be handled by resize logic on next frame
        } else if (result != VK_SUCCESS) {
            throw Exception{std::format("Failed to present swap chain image: {}", static_cast<int>(result))};
        }

        m_frameSync.advanceFrame();
    }
}

} // namespace R3

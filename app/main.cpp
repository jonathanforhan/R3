#include <cstdint>
#include <exception>
#include <format>
#include <iterator>
#include <vector>
#include <vulkan/vulkan_core.h>
#include <Exception.hpp>
#include <Log.hpp>
#include <render/Window.hpp>
#include <render/vulkan/vulkan-Buffer.hpp>
#include <render/vulkan/vulkan-Check.hpp>
#include <render/vulkan/vulkan-CommandAllocator.hpp>
#include <render/vulkan/vulkan-GraphicsPipeline.hpp>
#include <render/vulkan/vulkan-RenderContext.hpp>
#include <render/vulkan/vulkan-RenderPass.hpp>
#include <render/vulkan/vulkan-Shader.hpp>
#include <render/vulkan/vulkan-Swapchain.hpp>

using namespace R3;

// Triangle vertices - matches your vertex shader (vec3 position, vec3 color)
const Vertex vertices[3] = {
    {{0.0f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}}, // Top - Red
    {{-0.5f, 0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}}, // Bottom left - Green
    {{0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}},  // Bottom right - Blue
};

int main() {
#ifdef _WIN32
    detail::enableWindowsConsoleColors();
#endif

    try {
        // Create core objects
        Window window;
        RenderContext renderContext;
        Swapchain swapchain;

        // Create rendering objects
        RenderPass renderPass;
        Shader vertexShader;
        Shader fragmentShader;
        GraphicsPipeline graphicsPipeline;
        Buffer vertexBuffer;
        CommandAllocator commandAllocator;

        // Framebuffers and command buffers
        std::vector<VkFramebuffer> framebuffers;
        std::vector<VkCommandBuffer> commandBuffers;

        // Synchronization objects
        std::vector<VkSemaphore> imageAvailableSemaphores;
        std::vector<VkSemaphore> renderFinishedSemaphores;
        std::vector<VkFence> inFlightFences;

        constexpr int MAX_FRAMES_IN_FLIGHT = 2;
        int currentFrame                   = 0;

        // Initialize window and Vulkan
        window.create();
        window.show();
        renderContext.create(window);
        swapchain.create(renderContext, window);

        VkDevice device = renderContext.device();

        // Create render pass
        AttachmentDescription colorAttachment{
            .format         = swapchain.format(),
            .samples        = VK_SAMPLE_COUNT_1_BIT,
            .loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR,
            .storeOp        = VK_ATTACHMENT_STORE_OP_STORE,
            .stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
            .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
            .initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED,
            .finalLayout    = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
        };
        renderPass.create(renderContext, {colorAttachment});

        // Load shaders
        vertexShader.createFromFile(renderContext, "_spirv/basic.vert.spv", ShaderStage::Vertex);
        fragmentShader.createFromFile(renderContext, "_spirv/basic.frag.spv", ShaderStage::Fragment);

        // Create graphics pipeline
        graphicsPipeline.create(renderContext, renderPass, vertexShader, fragmentShader, swapchain.extent());

        // Create vertex buffer
        VkDeviceSize bufferSize = sizeof(vertices[0]) * std::size(vertices);
        vertexBuffer.create(renderContext,
                            bufferSize,
                            VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
        vertexBuffer.copyData(vertices);

        // Create framebuffers
        framebuffers.resize(swapchain.imageViews().size());
        for (size_t i = 0; i < swapchain.imageViews().size(); i++) {
            VkImageView attachments[] = {swapchain.imageViews()[i]};

            VkFramebufferCreateInfo framebufferInfo{
                .sType           = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
                .pNext           = nullptr,
                .flags           = 0,
                .renderPass      = renderPass.handle(),
                .attachmentCount = 1,
                .pAttachments    = attachments,
                .width           = swapchain.extent().width,
                .height          = swapchain.extent().height,
                .layers          = 1,
            };
            VK_CHECK(vkCreateFramebuffer(device, &framebufferInfo, nullptr, &framebuffers[i]));
        }

        // Create command pool and buffers
        commandAllocator.create(renderContext, renderContext.graphicsQueue().index, CommandPoolMode::Reset);
        commandBuffers = commandAllocator.allocateBuffers(MAX_FRAMES_IN_FLIGHT);

        // Create synchronization objects
        const size_t imageCount = swapchain.images().size();

        // Per-frame semaphores for acquire
        imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
        // Per-image semaphores for render finished
        renderFinishedSemaphores.resize(imageCount);
        // Per-frame fences
        inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

        VkSemaphoreCreateInfo semaphoreInfo{
            .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
        };

        VkFenceCreateInfo fenceInfo{
            .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
            .pNext = nullptr,
            .flags = VK_FENCE_CREATE_SIGNALED_BIT,
        };

        // Create per-frame acquire semaphores
        for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            VK_CHECK(vkCreateSemaphore(device, &semaphoreInfo, nullptr, &imageAvailableSemaphores[i]));
            VK_CHECK(vkCreateFence(device, &fenceInfo, nullptr, &inFlightFences[i]));
        }

        // Create per-image render finished semaphores
        for (size_t i = 0; i < imageCount; i++) {
            VK_CHECK(vkCreateSemaphore(device, &semaphoreInfo, nullptr, &renderFinishedSemaphores[i]));
        }

        LOG_INFO("R3 Engine initialized successfully");

        // Main render loop
        while (!window.shouldClose()) {
            window.update();

            // Handle window resize
            if (window.shouldResize()) {
                vkDeviceWaitIdle(device);

                // Destroy old framebuffers
                for (auto framebuffer : framebuffers) {
                    vkDestroyFramebuffer(device, framebuffer, nullptr);
                }

                // Destroy old per-image semaphores
                for (size_t i = 0; i < renderFinishedSemaphores.size(); i++) {
                    vkDestroySemaphore(device, renderFinishedSemaphores[i], nullptr);
                }

                swapchain.recreate(renderContext, window);

                // Recreate per-image semaphores for new swapchain
                const size_t newImageCount = swapchain.images().size();
                renderFinishedSemaphores.resize(newImageCount);

                VkSemaphoreCreateInfo semaphoreInfo{
                    .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
                    .pNext = nullptr,
                    .flags = 0,
                };

                for (size_t i = 0; i < newImageCount; i++) {
                    VK_CHECK(vkCreateSemaphore(device, &semaphoreInfo, nullptr, &renderFinishedSemaphores[i]));
                }

                // Recreate framebuffers
                framebuffers.resize(swapchain.imageViews().size());
                for (size_t i = 0; i < swapchain.imageViews().size(); i++) {
                    VkImageView attachments[] = {swapchain.imageViews()[i]};

                    VkFramebufferCreateInfo framebufferInfo{
                        .sType           = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
                        .pNext           = nullptr,
                        .flags           = 0,
                        .renderPass      = renderPass.handle(),
                        .attachmentCount = 1,
                        .pAttachments    = attachments,
                        .width           = swapchain.extent().width,
                        .height          = swapchain.extent().height,
                        .layers          = 1,
                    };
                    VK_CHECK(vkCreateFramebuffer(device, &framebufferInfo, nullptr, &framebuffers[i]));
                }

                window.setShouldResize(false);
                continue;
            }

            // Skip rendering if minimized
            if (window.isMinimized()) {
                continue;
            }

            // Wait for previous frame
            vkWaitForFences(device, 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);
            vkResetFences(device, 1, &inFlightFences[currentFrame]);

            // Acquire next image
            uint32_t imageIndex;
            VkResult result = swapchain.acquireNextImage(imageAvailableSemaphores[currentFrame], imageIndex);

            if (result == VK_ERROR_OUT_OF_DATE_KHR) {
                continue; // Will be handled by resize logic
            } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
                throw Exception{std::format("Failed to acquire swap chain image: {}", static_cast<int>(result))};
            }

            // Record command buffer
            VkCommandBuffer commandBuffer = commandBuffers[currentFrame];
            vkResetCommandBuffer(commandBuffer, 0);

            VkCommandBufferBeginInfo beginInfo{
                .sType            = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
                .pNext            = nullptr,
                .flags            = 0,
                .pInheritanceInfo = nullptr,
            };
            VK_CHECK(vkBeginCommandBuffer(commandBuffer, &beginInfo));

            // Begin render pass
            VkClearValue clearValue{{{1.0f, 0.0f, 1.0f, 1.0f}}};
            VkRenderPassBeginInfo renderPassInfo{
                .sType       = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
                .pNext       = nullptr,
                .renderPass  = renderPass.handle(),
                .framebuffer = framebuffers[imageIndex],
                .renderArea =
                    {
                        .offset = {0, 0},
                        .extent = swapchain.extent(),
                    },
                .clearValueCount = 1,
                .pClearValues    = &clearValue,
            };

            vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

            // Bind pipeline and draw
            graphicsPipeline.bind(commandBuffer);

            VkBuffer vertexBuffers[] = {vertexBuffer.handle()};
            VkDeviceSize offsets[]   = {0};
            vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);

            vkCmdDraw(commandBuffer, static_cast<uint32_t>(std::size(vertices)), 1, 0, 0);

            vkCmdEndRenderPass(commandBuffer);
            VK_CHECK(vkEndCommandBuffer(commandBuffer));

            // Submit command buffer - use per-frame acquire, per-image render finished
            VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
            VkSubmitInfo submitInfo{
                .sType                = VK_STRUCTURE_TYPE_SUBMIT_INFO,
                .pNext                = nullptr,
                .waitSemaphoreCount   = 1,
                .pWaitSemaphores      = &imageAvailableSemaphores[currentFrame],
                .pWaitDstStageMask    = waitStages,
                .commandBufferCount   = 1,
                .pCommandBuffers      = &commandBuffer,
                .signalSemaphoreCount = 1,
                .pSignalSemaphores    = &renderFinishedSemaphores[imageIndex], // Use imageIndex!
            };

            VK_CHECK(vkQueueSubmit(renderContext.graphicsQueue().handle, 1, &submitInfo, inFlightFences[currentFrame]));

            // Present - use per-image semaphore
            result = swapchain.present(
                renderContext.presentQueue().handle, renderFinishedSemaphores[imageIndex], imageIndex);

            if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
                // Will be handled by resize logic on next frame
            } else if (result != VK_SUCCESS) {
                throw Exception{std::format("Failed to present swap chain image: {}", static_cast<int>(result))};
            }

            currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
        }

        // Wait for device to finish
        vkDeviceWaitIdle(device);

        // Cleanup
        for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            vkDestroySemaphore(device, imageAvailableSemaphores[i], nullptr);
            vkDestroyFence(device, inFlightFences[i], nullptr);
        }

        for (size_t i = 0; i < renderFinishedSemaphores.size(); i++) {
            vkDestroySemaphore(device, renderFinishedSemaphores[i], nullptr);
        }

        for (auto framebuffer : framebuffers) {
            vkDestroyFramebuffer(device, framebuffer, nullptr);
        }

        commandAllocator.destroy();
        vertexBuffer.destroy();
        graphicsPipeline.destroy();
        fragmentShader.destroy();
        vertexShader.destroy();
        renderPass.destroy();
        swapchain.destroy();
        renderContext.destroy();
        window.destroy();

        LOG_INFO("R3 Engine shutdown complete");

    } catch (const Exception& ex) {
        LOG_ERROR("R3 Engine error: {}", ex.what());
        return -1;
    } catch (const std::exception& ex) {
        LOG_ERROR("Standard exception: {}", ex.what());
        return -1;
    }

    return 0;
}
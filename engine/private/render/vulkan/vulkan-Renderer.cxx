#if R3_VULKAN

#include "render/Renderer.hxx"
#include "render/Window.hpp"
#include "vulkan-ColorBuffer.hxx"
#include "vulkan-CommandPool.hxx"
#include "vulkan-DepthBuffer.hxx"
#include "vulkan-Fence.hxx"
#include "vulkan-Instance.hxx"
#include "vulkan-LogicalDevice.hxx"
#include "vulkan-PhysicalDevice.hxx"
#include "vulkan-RenderPass.hxx"
#include "vulkan-Semaphore.hxx"
#include "vulkan-ShaderModule.hxx"
#include "vulkan-Surface.hxx"
#include "vulkan-Swapchain.hxx"
#include <api/Log.hpp>
#include <api/Result.hpp>
#include <api/Types.hpp>
#include <cassert>
#include <cstdint>
#include <expected>
#include <type_traits>
#include <vector>
#include <vulkan/vulkan.h>

namespace R3 {
using namespace ::R3::vulkan;

Result<Renderer> Renderer::create(Window& window) {
    Renderer self;
    self.m_window = &window;

    // Instance Extensions
    std::vector<const char*> extensions(Instance::queryRequiredExtensions());
    std::vector<const char*> validationLayers;

    // Validation
#if R3_VALIDATION_LAYERS
    extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    validationLayers.push_back("VK_LAYER_KHRONOS_validation");
#endif

    // Instance
    Result<Instance> instance = Instance::create({
        .applicationName  = "R3 Game Engine",
        .extensions       = extensions,
        .validationLayers = validationLayers,
    });
    R3_PROPAGATE(instance);
    self.m_instance = std::move(instance.value());
    assert(self.m_instance.valid());

    // Surface
    Result<Surface> surface = Surface::create({
        .instance = self.m_instance,
        .window   = *self.m_window,
    });
    R3_PROPAGATE(surface);
    self.m_surface = std::move(surface.value());
    assert(self.m_surface.valid());

    // Physical Device
    Result<PhysicalDevice> physicalDevice = PhysicalDevice::query({
        .instance   = self.m_instance,
        .surface    = self.m_surface,
        .extensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME},
    });
    R3_PROPAGATE(physicalDevice);
    self.m_physicalDevice = std::move(physicalDevice.value());
    assert(self.m_physicalDevice.valid());

    // Logical Device and Queues
    Result<LogicalDevice> device = LogicalDevice::create({
        .instance       = self.m_instance,
        .surface        = self.m_surface,
        .physicalDevice = self.m_physicalDevice,
    });
    R3_PROPAGATE(device);
    self.m_device = std::move(device.value());
    assert(self.m_device.valid());

    // Swapchain
    Result<Swapchain> swapchain = Swapchain::create({
        .surface        = self.m_surface,
        .physicalDevice = self.m_physicalDevice,
        .device         = self.m_device,
        .window         = *self.m_window,
    });
    R3_PROPAGATE(swapchain);
    self.m_swapchain = std::move(swapchain.value());
    assert(self.m_swapchain.valid());

    // RenderPass
    Result<RenderPass> renderPass = RenderPass::create({
        .physicalDevice = self.m_physicalDevice,
        .device         = self.m_device,
        .colorAttachment =
            {
                .format      = self.m_swapchain.surfaceFormat(),
                .sampleCount = self.m_physicalDevice.sampleCount(),
            },
        .depthAttachment =
            {
                .sampleCount = self.m_physicalDevice.sampleCount(),
            },
    });
    R3_PROPAGATE(renderPass);
    self.m_renderPass = std::move(renderPass.value());
    assert(self.m_renderPass.valid());

    // ColorBuffer
    // DepthBuffer
    // Framebuffers
    // recreating the swapchain sets attachment buffers and framebuffers
    Result<void> swapchainRecreateResult = self.m_swapchain.recreate({
        .window         = *self.m_window,
        .surface        = self.m_surface,
        .physicalDevice = self.m_physicalDevice,
        .device         = self.m_device,
        .framebuffers   = self.m_framebuffers,
        .colorBuffer    = self.m_colorBuffer,
        .depthBuffer    = self.m_depthBuffer,
        .renderPass     = self.m_renderPass,
    });
    R3_PROPAGATE(swapchainRecreateResult);
    assert(self.m_colorBuffer.valid());
    assert(self.m_depthBuffer.valid());
    assert(self.m_swapchain.valid());

    // CommandPool and CommandBuffers
    Result<CommandPool> commandPool = CommandPool::create({
        .device             = self.m_device,
        .queue              = self.m_device.graphicsQueue(),
        .flags              = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
        .commandBufferCount = MAX_FRAMES_IN_FLIGHT,
    });
    R3_PROPAGATE(commandPool);
    self.m_commandPool = std::move(commandPool.value());
    assert(self.m_commandPool.valid());

    // Synchronization
    for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        self.m_imageAvailable[i] = Semaphore(self.m_device);
        self.m_renderFinished[i] = Semaphore(self.m_device);
        self.m_inFlight[i]       = Fence(self.m_device);
    }

    // TODO FIXME
    // {
    self.m_vertexShader = *ShaderModule::create({
        .device = self.m_device,
        .path   = "spirv/basic.vert.spv",
        .stage  = VK_SHADER_STAGE_VERTEX_BIT,
    });

    self.m_fragmentShader = *ShaderModule::create({
        .device = self.m_device,
        .path   = "spirv/basic.frag.spv",
        .stage  = VK_SHADER_STAGE_FRAGMENT_BIT,
    });

    const VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
    };

    VkResult result = vkCreateDescriptorSetLayout(
        self.m_device.vk(), &descriptorSetLayoutCreateInfo, nullptr, &self.m_descriptorSetLayout);

    if (result != VK_SUCCESS) {
        return std::unexpected(Error::InitializationFailure);
    }

    self.m_graphicsPipeline = *GraphicsPipeline::create({
        .physicalDevice        = self.m_physicalDevice,
        .device                = self.m_device,
        .swapchain             = self.m_swapchain,
        .renderPass            = self.m_renderPass,
        .descriptorSetLayout   = self.m_descriptorSetLayout,
        .bindingDescriptions   = {},
        .attributeDescriptions = {},
        .vertexShader          = std::move(self.m_vertexShader),
        .fragmentShader        = std::move(self.m_fragmentShader),
        .msaa                  = true,
    });
    // }

    return self;
}

Renderer::~Renderer() {
    if (m_device.valid()) {
        vkDeviceWaitIdle(m_device.vk());
        /* TODO FIXME --> */ vkDestroyDescriptorSetLayout(m_device.vk(), m_descriptorSetLayout, nullptr);
    }
}

void Renderer::render() {
    VkFence inFlight = m_inFlight[m_currentFrame].vk();

    VkResult result = vkWaitForFences(m_device.vk(), 1, &inFlight, VK_FALSE, UINT64_MAX);
    assert(result == VK_SUCCESS);

    VkSemaphore imageAvailable = m_imageAvailable[m_currentFrame].vk();

    uint32 imageIndex;
    result =
        vkAcquireNextImageKHR(m_device.vk(), m_swapchain.vk(), UINT64_MAX, imageAvailable, VK_NULL_HANDLE, &imageIndex);

    [[unlikely]] if (result != VK_SUCCESS) {
        if (result == VK_ERROR_OUT_OF_DATE_KHR) {
            resize();
            return;
        } else if (result != VK_SUBOPTIMAL_KHR) {
            R3_LOG(Error, "vulkan error code: {}", (int)result);
        }
    }

    (void)vkResetFences(m_device.vk(), 1, &inFlight);

    VkCommandBuffer cmd = m_commandPool[m_currentFrame];

    (void)vkResetCommandBuffer(cmd, 0);

    VkCommandBufferBeginInfo commandBufferBeginInfo = {
        .sType            = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .pNext            = nullptr,
        .flags            = {},
        .pInheritanceInfo = nullptr,
    };
    (void)vkBeginCommandBuffer(cmd, &commandBufferBeginInfo);
    { // COMMAND BUFFER

        static constexpr VkClearValue clearValues[] = {
            VkClearValue{.color = {0.0f, 0.0f, 0.0f, 1.0f}},             // clear color
            VkClearValue{.depthStencil = {.depth = 1.0f, .stencil = 0}}, // clear depth stencil
        };

        VkRenderPassBeginInfo renderPassBeginInfo = {
            .sType       = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
            .pNext       = nullptr,
            .renderPass  = m_renderPass.vk(),
            .framebuffer = m_framebuffers[imageIndex].vk(), // NOTE we use the imageIndex NOT m_currentFrame
            .renderArea =
                {
                    .offset = {},
                    .extent = m_swapchain.extent(),
                },
            .clearValueCount = static_cast<uint32>(std::size(clearValues)),
            .pClearValues    = clearValues,
        };
        vkCmdBeginRenderPass(cmd, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
        { // RENDER PASS

            vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, m_graphicsPipeline.vk());

            const VkViewport viewport = {
                .x        = 0.0f,
                .y        = static_cast<float>(m_swapchain.extent().height),
                .width    = static_cast<float>(m_swapchain.extent().width),
                .height   = -static_cast<float>(m_swapchain.extent().height),
                .minDepth = 0.0f,
                .maxDepth = 1.0f,
            };
            vkCmdSetViewport(cmd, 0, 1, &viewport);

            const VkRect2D scissor = {
                .offset = {},
                .extent = m_swapchain.extent(),
            };
            vkCmdSetScissor(cmd, 0, 1, &scissor);

            vkCmdDraw(cmd, 3, 1, 0, 0);
        }
        vkCmdEndRenderPass(cmd);
    }
    (void)vkEndCommandBuffer(cmd);

    VkSemaphore imageReady[]        = {imageAvailable};
    VkSemaphore renderFinished[]    = {m_renderFinished[m_currentFrame].vk()};
    VkPipelineStageFlags waitStages = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

    VkSubmitInfo submitInfo = {
        .sType                = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .pNext                = nullptr,
        .waitSemaphoreCount   = static_cast<uint32>(std::size(imageReady)),
        .pWaitSemaphores      = imageReady,
        .pWaitDstStageMask    = &waitStages,
        .commandBufferCount   = 1,
        .pCommandBuffers      = &cmd,
        .signalSemaphoreCount = static_cast<uint32>(std::size(renderFinished)),
        .pSignalSemaphores    = renderFinished,
    };
    (void)vkQueueSubmit(m_device.graphicsQueue().vk(), 1, &submitInfo, inFlight);

    VkSwapchainKHR swapchain = m_swapchain.vk();

    VkPresentInfoKHR presentInfo = {
        .sType              = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
        .pNext              = nullptr,
        .waitSemaphoreCount = static_cast<uint32>(std::size(renderFinished)),
        .pWaitSemaphores    = renderFinished,
        .swapchainCount     = 1,
        .pSwapchains        = &swapchain,
        .pImageIndices      = &imageIndex,
        .pResults           = nullptr,
    };

    result = vkQueuePresentKHR(m_device.presentationQueue().vk(), &presentInfo);

    if (result != VK_SUCCESS) {
        if (result == VK_SUBOPTIMAL_KHR || result == VK_ERROR_OUT_OF_DATE_KHR || m_window->shouldResize()) {
            resize();
        } else {
            R3_LOG(Error, "vkPresent error: {}", (int)result);
        }
    }

    m_currentFrame = (m_currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

void Renderer::resize() {
    Result<void> result = m_swapchain.recreate({
        .window         = *m_window,
        .surface        = m_surface,
        .physicalDevice = m_physicalDevice,
        .device         = m_device,
        .framebuffers   = m_framebuffers,
        .colorBuffer    = m_colorBuffer,
        .depthBuffer    = m_depthBuffer,
        .renderPass     = m_renderPass,
    });

    if (!result) {
        R3_LOG(Error, "failed to resize swapchain");
        return;
    }

    m_window->setShouldResize(false);
}

} // namespace R3

#endif // R3_VULKAN

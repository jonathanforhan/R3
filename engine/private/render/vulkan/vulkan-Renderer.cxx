#if R3_VULKAN

#include "render/Renderer.hpp"
// clang-format off
#include "vulkan/vulkan.h"
#include "GLFW/glfw3.h"
// clang-format on
#include "api/Check.hpp"
#include "api/Ensure.hpp"
#include "api/Log.hpp"

namespace R3 {

void Renderer::create(RendererSpecification spec) {
    CHECK(spec.window != nullptr);
    m_spec = spec;

    //--- Instance Extensions
    uint32 extensionCount = 0;
    const char** extensions_ = glfwGetRequiredInstanceExtensions(&extensionCount);
    std::vector<const char*> extensions(extensions_, extensions_ + extensionCount);
    std::vector<const char*> validationLayers;

    //--- Validation
#if R3_VALIDATION_LAYERS_ENABLED
    extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    validationLayers.push_back("VK_LAYER_KHRONOS_validation");
#endif

    //--- Instance
    m_instance.create({
        .applicationName = "R3 Game Engine",
        .extensions = extensions,
        .validationLayers = validationLayers,
    });

    //--- Surface
    m_surface.create({
        .instance = &m_instance,
        .window = m_spec.window,
    });

    //--- Physical Device
    m_physicalDevice.select({
        .instance = &m_instance,
        .surface = &m_surface,
        .extensions =
            {
                VK_KHR_SWAPCHAIN_EXTENSION_NAME,
            },
    });

    //--- Logical Device and Queues
    m_logicalDevice.create({
        .instance = &m_instance,
        .surface = &m_surface,
        .physicalDevice = &m_physicalDevice,
    });

    //--- Swapchain
    m_swapchain.create({
        .physicalDevice = &m_physicalDevice,
        .surface = &m_surface,
        .logicalDevice = &m_logicalDevice,
        .window = spec.window,
    });

    //--- RenderPass
    m_renderPass.create({
        .logicalDevice = &m_logicalDevice,
        .swapchain = &m_swapchain,
    });

    //--- Pipeline Layout
    m_pipelineLayout.create({
        .logicalDevice = &m_logicalDevice,
    });

    //--- Graphics Pipeline
    m_graphicsPipeline.create({
        .logicalDevice = &m_logicalDevice,
        .swapchain = &m_swapchain,
        .renderPass = &m_renderPass,
        .pipelineLayout = &m_pipelineLayout,
        .vertexShaderPath = "spirv/test.vert.spv",
        .fragmentShaderPath = "spirv/test.frag.spv",
    });

    //--- Framebuffers
    for (const auto& swapchainImageView : m_swapchain.imageViews()) {
        Framebuffer& framebuffer = m_framebuffers.emplace_back();
        framebuffer.create({
            .logicalDevice = &m_logicalDevice,
            .swapchain = &m_swapchain,
            .imageView = &swapchainImageView,
            .renderPass = &m_renderPass,
        });
    }

    //--- CommandPool and CommandBuffer
    m_commandPool.create({
        .logicalDevice = &m_logicalDevice,
        .swapchain = &m_swapchain,
        .commandBufferCount = detail::MAX_FRAMES_IN_FLIGHT,
    });

    //--- Synchronization
    for (uint32 i = 0; i < detail::MAX_FRAMES_IN_FLIGHT; i++) {
        m_imageAvailable[i].create({.logicalDevice = &m_logicalDevice});
        m_renderFinished[i].create({.logicalDevice = &m_logicalDevice});
        m_inFlight[i].create({.logicalDevice = &m_logicalDevice});
    }
}

void Renderer::destroy() {
    vkDeviceWaitIdle(m_logicalDevice.handle<VkDevice>());

    for (uint32 i = 0; i < detail::MAX_FRAMES_IN_FLIGHT; i++) {
        m_imageAvailable[i].destroy();
        m_renderFinished[i].destroy();
        m_inFlight[i].destroy();
    }

    m_commandPool.destroy();
    for (auto& framebuffer : m_framebuffers)
        framebuffer.destroy();
    m_graphicsPipeline.destroy();
    m_pipelineLayout.destroy();
    m_renderPass.destroy();
    m_swapchain.destroy();
    m_logicalDevice.destroy();
    m_surface.destroy();
    m_instance.destroy();
}

void Renderer::render() {
    VkResult result;

    const VkFence fences[]{m_inFlight[m_currentFrame].handle<VkFence>()};
    vkWaitForFences(m_logicalDevice.handle<VkDevice>(), 1, fences, VK_TRUE, UINT64_MAX);

    uint32 imageIndex;
    result = vkAcquireNextImageKHR(m_logicalDevice.handle<VkDevice>(),
                                   m_swapchain.handle<VkSwapchainKHR>(),
                                   UINT64_MAX,
                                   m_imageAvailable[m_currentFrame].handle<VkSemaphore>(),
                                   VK_NULL_HANDLE,
                                   &imageIndex);

    if (result != VK_SUCCESS) {
        if (result == VK_ERROR_OUT_OF_DATE_KHR) {
            m_swapchain.recreate(m_framebuffers, m_renderPass);
            return;
        } else if (result != VK_SUBOPTIMAL_KHR) {
            ENSURE(false);
        }
    }

    vkResetFences(m_logicalDevice.handle<VkDevice>(), 1, fences);

    CommandBuffer& commandBuffer = m_commandPool.commandBuffers()[m_currentFrame];
    commandBuffer.resetCommandBuffer();
    commandBuffer.beginCommandBuffer();
    {
        commandBuffer.beginRenderPass(m_renderPass, m_framebuffers[imageIndex]);
        {
            commandBuffer.bindPipeline(m_graphicsPipeline);
            vkCmdDraw(commandBuffer.handle<VkCommandBuffer>(), 3, 1, 0, 0);
        }
        commandBuffer.endRenderPass();
    }
    commandBuffer.endCommandBuffer();

    VkSemaphore waitSemaphores[] = {m_imageAvailable[m_currentFrame].handle<VkSemaphore>()};
    VkSemaphore singalSemaphores[] = {m_renderFinished[m_currentFrame].handle<VkSemaphore>()};
    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    VkCommandBuffer commandBuffers[] = {commandBuffer.handle<VkCommandBuffer>()};
    VkSubmitInfo submitInfo = {
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .pNext = nullptr,
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = waitSemaphores,
        .pWaitDstStageMask = waitStages,
        .commandBufferCount = 1,
        .pCommandBuffers = commandBuffers,
        .signalSemaphoreCount = 1,
        .pSignalSemaphores = singalSemaphores,
    };

    result = vkQueueSubmit(m_logicalDevice.graphicsQueue().handle<VkQueue>(),
                           1,
                           &submitInfo,
                           m_inFlight[m_currentFrame].handle<VkFence>());
    CHECK(result == VK_SUCCESS);

    VkSwapchainKHR swapchains[]{m_swapchain.handle<VkSwapchainKHR>()};
    VkPresentInfoKHR presentInfo = {
        .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
        .pNext = nullptr,
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = singalSemaphores,
        .swapchainCount = 1,
        .pSwapchains = swapchains,
        .pImageIndices = &imageIndex,
        .pResults = nullptr,
    };

    result = vkQueuePresentKHR(m_logicalDevice.presentattionQueue().handle<VkQueue>(), &presentInfo);
    if (result != VK_SUCCESS) {
        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || m_spec.window->shouldResize()) {
            m_spec.window->setShouldResize(false);
            m_swapchain.recreate(m_framebuffers, m_renderPass);
        } else if (result != VK_SUBOPTIMAL_KHR) {
            ENSURE(false);
        }
    }

    m_currentFrame = (m_currentFrame + 1) % detail::MAX_FRAMES_IN_FLIGHT;
}

} // namespace R3

#endif // R3_VULKAN

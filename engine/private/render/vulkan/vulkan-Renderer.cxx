#if R3_VULKAN

#include "render/Renderer.hpp"
// clang-format off
#include "vulkan/vulkan.hpp"
#include "GLFW/glfw3.h"
// clang-format on
#include "api/Check.hpp"
#include "api/Ensure.hpp"
#include "api/Log.hpp"
#include "core/BasicGeometry.hpp"

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
        .flags = CommandPoolFlags::Reset,
        .commandBufferCount = detail::MAX_FRAMES_IN_FLIGHT,
    });
    m_commandPoolTransient.create({
        .logicalDevice = &m_logicalDevice,
        .swapchain = &m_swapchain,
        .flags = CommandPoolFlags::Reset,
        .commandBufferCount = 1,
    });

    //--- Synchronization
    for (uint32 i = 0; i < detail::MAX_FRAMES_IN_FLIGHT; i++) {
        m_imageAvailable[i].create({.logicalDevice = &m_logicalDevice});
        m_renderFinished[i].create({.logicalDevice = &m_logicalDevice});
        m_inFlight[i].create({.logicalDevice = &m_logicalDevice});
    }

    //--- Test
    #if 0
    Vertex vertices[36];
    Cube(vertices);

        // Vertex Buffer
    auto& vbuf = m_vertexBuffers.emplace_back();
    vbuf.create({
        .physicalDevice = &m_physicalDevice,
        .logicalDevice = &m_logicalDevice,
        .commandPool = &m_commandPoolTransient,
        .vertices = vertices,
    });
    #else
    Vertex vertices[8];
    uint16 indices[36];
    IndexedCube(vertices, indices);
    auto& v = m_vertexBuffers.emplace_back();
    v.create({
        .physicalDevice = &m_physicalDevice,
        .logicalDevice = &m_logicalDevice,
        .commandPool = &m_commandPoolTransient,
        .vertices = vertices,
    });
    auto& i = m_indexBuffers.emplace_back();
    i.create({
        .physicalDevice = &m_physicalDevice,
        .logicalDevice = &m_logicalDevice,
        .commandPool = &m_commandPoolTransient,
        .indices = indices,
    });
    #endif
}

void Renderer::destroy() {
    vkDeviceWaitIdle(m_logicalDevice.handle<VkDevice>());

    for (auto& indexBuffer : m_indexBuffers) {
        indexBuffer.destroy();
    }

    for (auto& vertexBuffer : m_vertexBuffers) {
        vertexBuffer.destroy();
    }

    for (uint32 i = 0; i < detail::MAX_FRAMES_IN_FLIGHT; i++) {
        m_imageAvailable[i].destroy();
        m_renderFinished[i].destroy();
        m_inFlight[i].destroy();
    }

    m_commandPool.destroy();
    m_commandPoolTransient.destroy();

    for (auto& framebuffer : m_framebuffers) {
        framebuffer.destroy();
    }

    m_graphicsPipeline.destroy();
    m_pipelineLayout.destroy();
    m_renderPass.destroy();
    m_swapchain.destroy();
    m_logicalDevice.destroy();
    m_surface.destroy();
    m_instance.destroy();
}

void Renderer::render() {
    const vk::Fence fences[]{m_inFlight[m_currentFrame].as<vk::Fence>()};
    (void)m_logicalDevice.as<vk::Device>().waitForFences(fences, vk::True, UINT64_MAX);

    auto semaphore = m_imageAvailable[m_currentFrame].as<vk::Semaphore>();
    auto [result, value] =
        m_logicalDevice.as<vk::Device>().acquireNextImageKHR(m_swapchain.as<vk::SwapchainKHR>(), UINT64_MAX, semaphore);
    uint32 imageIndex = value;

    if (result != vk::Result::eSuccess) {
        if (result == vk::Result::eErrorOutOfDateKHR) {
            m_swapchain.recreate(m_framebuffers, m_renderPass);
            return;
        } else if (result != vk::Result::eSuboptimalKHR) {
            ENSURE(false);
        }
    }

    m_logicalDevice.as<vk::Device>().resetFences(fences);

    CommandBuffer& commandBuffer = m_commandPool.commandBuffers()[m_currentFrame];
    commandBuffer.resetCommandBuffer();
    commandBuffer.beginCommandBuffer();
    {
        commandBuffer.beginRenderPass(m_renderPass, m_framebuffers[imageIndex]);
        {
            commandBuffer.bindPipeline(m_graphicsPipeline);
            commandBuffer.bindVertexBuffers(m_vertexBuffers);
            commandBuffer.bindIndexBuffer(m_indexBuffers.front());
            commandBuffer.as<vk::CommandBuffer>().drawIndexed(m_indexBuffers.front().indexCount(), 1, 0, 0, 0);
        }
        commandBuffer.endRenderPass();
    }
    commandBuffer.endCommandBuffer();

    vk::Semaphore waitSemaphores[] = {m_imageAvailable[m_currentFrame].as<vk::Semaphore>()};
    vk::Semaphore singalSemaphores[] = {m_renderFinished[m_currentFrame].as<vk::Semaphore>()};
    vk::PipelineStageFlags waitStages[] = {vk::PipelineStageFlagBits::eColorAttachmentOutput};
    vk::CommandBuffer commandBuffers[] = {commandBuffer.as<vk::CommandBuffer>()};
    vk::SubmitInfo submitInfo = {
        .sType = vk::StructureType::eSubmitInfo,
        .pNext = nullptr,
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = waitSemaphores,
        .pWaitDstStageMask = waitStages,
        .commandBufferCount = 1,
        .pCommandBuffers = commandBuffers,
        .signalSemaphoreCount = 1,
        .pSignalSemaphores = singalSemaphores,
    };

    m_logicalDevice.graphicsQueue().as<vk::Queue>().submit(submitInfo, m_inFlight[m_currentFrame].as<vk::Fence>());

    vk::SwapchainKHR swapchains[]{m_swapchain.as<vk::SwapchainKHR>()};
    vk::PresentInfoKHR presentInfo = {
        .sType = vk::StructureType::ePresentInfoKHR,
        .pNext = nullptr,
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = singalSemaphores,
        .swapchainCount = 1,
        .pSwapchains = swapchains,
        .pImageIndices = &imageIndex,
        .pResults = nullptr,
    };

    try {
        result = m_logicalDevice.presentationQueue().as<vk::Queue>().presentKHR(presentInfo);
    } catch (...) {
        if (result == vk::Result::eErrorOutOfDateKHR || result == vk::Result::eSuboptimalKHR ||
            m_spec.window->shouldResize()) {
            m_spec.window->setShouldResize(false);
            m_swapchain.recreate(m_framebuffers, m_renderPass);
        } else if (result != vk::Result::eSuboptimalKHR) {
            ENSURE(false);
        }
    }

    m_currentFrame = (m_currentFrame + 1) % detail::MAX_FRAMES_IN_FLIGHT;
}

} // namespace R3

#endif // R3_VULKAN

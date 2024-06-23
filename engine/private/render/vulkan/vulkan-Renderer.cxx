#if R3_VULKAN

#include "render/Renderer.hxx"
#include "render/Window.hpp"
#include "vulkan-ColorBuffer.hxx"
#include "vulkan-CommandBuffer.hxx"
#include "vulkan-CommandPool.hxx"
#include "vulkan-DepthBuffer.hxx"
#include "vulkan-Fence.hxx"
#include "vulkan-Instance.hxx"
#include "vulkan-LogicalDevice.hxx"
#include "vulkan-PhysicalDevice.hxx"
#include "vulkan-RenderPass.hxx"
#include "vulkan-Semaphore.hxx"
#include "vulkan-Surface.hxx"
#include "vulkan-Swapchain.hxx"
#include <api/Assert.hpp>
#include <api/Log.hpp>
#include <api/Types.hpp>
#include <cstdint>
#include <span>
#include <vector>
#include <vulkan/vulkan.h>

namespace R3 {
using namespace ::R3::vulkan;

Renderer::Renderer(Window& window)
    : m_window(window) {
    // Instance Extensions
    std::vector<const char*> extensions(Instance::queryRequiredExtensions());
    std::vector<const char*> validationLayers;

    // Validation
#if R3_VALIDATION_LAYERS
    extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    validationLayers.push_back("VK_LAYER_KHRONOS_validation");
#endif

    // Instance
    m_instance = Instance({
        .applicationName  = "R3 Game Engine",
        .extensions       = extensions,
        .validationLayers = validationLayers,
    });

    // Surface
    m_surface = Surface({
        .instance = m_instance,
        .window   = m_window,
    });

    // Physical Device
    m_physicalDevice = PhysicalDevice({
        .instance   = m_instance,
        .surface    = m_surface,
        .extensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME},
    });

    // Logical Device and Queues
    m_device = {{
        .instance       = m_instance,
        .surface        = m_surface,
        .physicalDevice = m_physicalDevice,
    }};

    // Swapchain
    m_swapchain = Swapchain({
        .surface        = m_surface,
        .physicalDevice = m_physicalDevice,
        .device         = m_device,
        .window         = m_window,
    });

    // RenderPass
    m_renderPass = RenderPass({
        .physicalDevice = m_physicalDevice,
        .device         = m_device,
        .colorAttachment =
            {
                .format      = m_swapchain.surfaceFormat(),
                .sampleCount = m_physicalDevice.sampleCount(),
            },
        .depthAttachment =
            {
                .sampleCount = m_physicalDevice.sampleCount(),
            },
    });

    // ColorBuffer
    // DepthBuffer
    // Framebuffers
    // recreating the swapchain sets attachment buffers and framebuffers
    m_swapchain.recreate({
        .window         = m_window,
        .surface        = m_surface,
        .physicalDevice = m_physicalDevice,
        .device         = m_device,
        .framebuffers   = m_framebuffers,
        .colorBuffer    = m_colorBuffer,
        .depthBuffer    = m_depthBuffer,
        .renderPass     = m_renderPass,
    });

    // CommandPool and CommandBuffers
    m_commandPool = CommandPool({
        .device             = m_device,
        .queue              = m_device.graphicsQueue(),
        .flags              = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
        .commandBufferCount = MAX_FRAMES_IN_FLIGHT,
    });

    // Synchronization
    for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        m_imageAvailable[i] = Semaphore(m_device);
        m_renderFinished[i] = Semaphore(m_device);
        m_inFlight[i]       = Fence(m_device);
    }

    // TODO FIXME
    // {
    m_vertexShader = ShaderModule({
        .device = m_device,
        .path   = "spirv/basic.vert.spv",
        .stage  = VK_SHADER_STAGE_VERTEX_BIT,
    });

    m_fragmentShader = ShaderModule({
        .device = m_device,
        .path   = "spirv/basic.frag.spv",
        .stage  = VK_SHADER_STAGE_FRAGMENT_BIT,
    });

    const VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
    };

    VkResult result =
        vkCreateDescriptorSetLayout(m_device.vk(), &descriptorSetLayoutCreateInfo, nullptr, &m_descriptorSetLayout);
    ENSURE(result == VK_SUCCESS);

    m_graphicsPipeline = GraphicsPipeline({
        .physicalDevice        = m_physicalDevice,
        .device                = m_device,
        .swapchain             = m_swapchain,
        .renderPass            = m_renderPass,
        .descriptorSetLayout   = m_descriptorSetLayout,
        .bindingDescription    = {},
        .attributeDescriptions = {},
        .vertexShader          = std::move(m_vertexShader),
        .fragmentShader        = std::move(m_fragmentShader),
        .msaa                  = true,
    });
    // }
}

Renderer::~Renderer() {
    vkDeviceWaitIdle(m_device.vk());
    /* TODO FIXME --> */ vkDestroyDescriptorSetLayout(m_device.vk(), m_descriptorSetLayout, nullptr);
}

void Renderer::render() {
    // Synchronization
    // {
    VkFence inFlight = m_inFlight[m_currentFrame].vk();

    VkResult result = vkWaitForFences(m_device.vk(), 1, &inFlight, VK_FALSE, UINT64_MAX);
    ASSERT(result == VK_SUCCESS);

    VkSemaphore imageAvailable = m_imageAvailable[m_currentFrame].vk();

    uint32 imageIndex;
    result =
        vkAcquireNextImageKHR(m_device.vk(), m_swapchain.vk(), UINT64_MAX, imageAvailable, VK_NULL_HANDLE, &imageIndex);

    [[unlikely]] if (result != VK_SUCCESS) {
        if (result == VK_ERROR_OUT_OF_DATE_KHR) {
            resize();
            return;
        } else if (result != VK_SUBOPTIMAL_KHR) {
            LOG(Error, "vulkan error code: {}", (int)result);
        }
    }

    m_inFlight[m_currentFrame].reset();
    // }

    // Setup
    // {
    // }

    // Render Pass
    // {
    CommandBuffer cmd = m_commandPool[m_currentFrame];
    cmd.reset();
    cmd.begin();
    cmd.beginRenderPass(m_renderPass, m_framebuffers[imageIndex], m_swapchain.extent());
    {
        cmd.bindPipeline(m_graphicsPipeline);
        cmd.setViewport(m_swapchain);
        cmd.setScissor(m_swapchain);
        vkCmdDraw(cmd.vk(), 3, 1, 0, 0);
    }
    cmd.endRenderPass();
    cmd.end();

    VkSemaphore imageReady[]     = {imageAvailable};
    VkSemaphore renderFinished[] = {m_renderFinished[m_currentFrame].vk()};

    cmd.submit({
        .device           = m_device,
        .waitStages       = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
        .waitSemaphores   = imageReady,
        .signalSemaphores = renderFinished,
        .fence            = inFlight,
    });

    result = cmd.present({
        .device         = m_device,
        .swapchain      = m_swapchain,
        .waitSemaphores = renderFinished,
        .currentImage   = imageIndex,
    });
    // }

    if (result != VK_SUCCESS) {
        if (result == VK_SUBOPTIMAL_KHR || result == VK_ERROR_OUT_OF_DATE_KHR || m_window.shouldResize()) {
            resize();
        } else {
            LOG(Error, "vkPresent error: {}", (int)result);
        }
    }

    m_currentFrame = (m_currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

void Renderer::resize() {
    m_swapchain.recreate({
        .window         = m_window,
        .surface        = m_surface,
        .physicalDevice = m_physicalDevice,
        .device         = m_device,
        .framebuffers   = m_framebuffers,
        .colorBuffer    = m_colorBuffer,
        .depthBuffer    = m_depthBuffer,
        .renderPass     = m_renderPass,
    });
    m_window.setShouldResize(false);
}

} // namespace R3

#endif // R3_VULKAN

#if R3_VULKAN

#include "render/Renderer.hpp"

#include <chrono>
// clang-format off
#include <vulkan/vulkan.hpp>
#include <GLFW/glfw3.h>
// clang-format on
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "api/Check.hpp"
#include "api/Ensure.hpp"
#include "api/Log.hpp"
#include "core/BasicGeometry.hpp"
#include "render/UniformBufferObject.hpp"

namespace R3 {

Renderer::Renderer(RendererSpecification spec)
    : m_spec(spec) {
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
    m_instance = Instance({
        .applicationName = "R3 Game Engine",
        .extensions = extensions,
        .validationLayers = validationLayers,
    });

    //--- Surface
    m_surface = Surface({
        .instance = &m_instance,
        .window = &m_spec.window,
    });

    //--- Physical Device
    m_physicalDevice = PhysicalDevice({
        .instance = &m_instance,
        .surface = &m_surface,
        .extensions =
            {
                VK_KHR_SWAPCHAIN_EXTENSION_NAME,
            },
    });

    //--- Logical Device and Queues
    m_logicalDevice = LogicalDevice({
        .instance = &m_instance,
        .surface = &m_surface,
        .physicalDevice = &m_physicalDevice,
    });

    //--- Swapchain
    m_swapchain = Swapchain({
        .physicalDevice = &m_physicalDevice,
        .surface = &m_surface,
        .logicalDevice = &m_logicalDevice,
        .window = &spec.window,
    });

    //--- RenderPass
    m_renderPass = RenderPass({
        .logicalDevice = &m_logicalDevice,
        .swapchain = &m_swapchain,
    });

    //--- DescriptorSetLayout
    m_descriptorSetLayout = DescriptorSetLayout({
        .logicalDevice = &m_logicalDevice,
    });

    //--- DescriptorPool
    m_descriptorPool = DescriptorPool({
        .logicalDevice = &m_logicalDevice,
        .descriptorSetLayout = &m_descriptorSetLayout,
        .descriptorSetCount = MAX_FRAMES_IN_FLIGHT,
    });

    //--- Pipeline Layout
    m_pipelineLayout = PipelineLayout({
        .logicalDevice = &m_logicalDevice,
        .descriptorSetLayout = &m_descriptorSetLayout,
    });

    //--- Graphics Pipeline
    m_graphicsPipeline = GraphicsPipeline({
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
        framebuffer = Framebuffer({
            .logicalDevice = &m_logicalDevice,
            .swapchain = &m_swapchain,
            .imageView = &swapchainImageView,
            .renderPass = &m_renderPass,
        });
    }

    //--- CommandPool and CommandBuffer
    m_commandPool = CommandPool({
        .logicalDevice = &m_logicalDevice,
        .swapchain = &m_swapchain,
        .flags = CommandPoolFlags::Reset,
        .commandBufferCount = MAX_FRAMES_IN_FLIGHT,
    });
    m_commandPoolTransient = CommandPool({
        .logicalDevice = &m_logicalDevice,
        .swapchain = &m_swapchain,
        .flags = CommandPoolFlags::Reset,
        .commandBufferCount = 1,
    });

    //--- Synchronization
    for (uint32 i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        m_imageAvailable[i] = Semaphore({.logicalDevice = &m_logicalDevice});
        m_renderFinished[i] = Semaphore({.logicalDevice = &m_logicalDevice});
        m_inFlight[i] = Fence({.logicalDevice = &m_logicalDevice});
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
    uint32 indices32[36];
    for (uint32 i = 0; i < 36; i++)
        indices32[i] = indices[i];

    auto& vertex = m_vertexBuffers.emplace_back();
    vertex = VertexBuffer({
        .physicalDevice = &m_physicalDevice,
        .logicalDevice = &m_logicalDevice,
        .commandPool = &m_commandPoolTransient,
        .vertices = vertices,
    });
    auto& index = m_indexBuffers.emplace_back();
    index = IndexBuffer<uint32>({
        .physicalDevice = &m_physicalDevice,
        .logicalDevice = &m_logicalDevice,
        .commandPool = &m_commandPoolTransient,
        .indices = indices32,
    });
#endif

    //--- Uniforms
    m_uniformBuffers.resize(MAX_FRAMES_IN_FLIGHT);
    for (auto& uniformBuffer : m_uniformBuffers) {
        uniformBuffer = UniformBuffer({
            .physicalDevice = &m_physicalDevice,
            .logicalDevice = &m_logicalDevice,
            .bufferSize = sizeof(UniformBufferObject),
        });
    }

    auto descriptorSets = m_descriptorPool.descriptorSets();
    for (uint32 i = 0; i < m_uniformBuffers.size(); i++) {
        descriptorSets[i].bindUniform(m_uniformBuffers[i], 0);
    }

    //--- Texture
    m_textureBuffer = TextureBuffer(TextureBufferSpecification{
        .physicalDevice = &m_physicalDevice,
        .logicalDevice = &m_logicalDevice,
        .commandPool = &m_commandPoolTransient,
        .path = "textures/container.jpg",
    });
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
            commandBuffer.bindDescriptorSet(m_pipelineLayout, m_descriptorPool.descriptorSet(m_currentFrame));
            commandBuffer.as<vk::CommandBuffer>().drawIndexed(m_indexBuffers.front().indexCount(), 1, 0, 0, 0);
        }
        commandBuffer.endRenderPass();
    }
    commandBuffer.endCommandBuffer();

#if 1
    static auto startTime = std::chrono::high_resolution_clock::now();
    auto currentTime = std::chrono::high_resolution_clock::now();
    float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();
    UniformBufferObject ubo{};
    ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    ubo.projection =
        glm::perspective(glm::radians(45.0f), m_swapchain.extent().x / (float)m_swapchain.extent().y, 0.1f, 10.0f);
    m_uniformBuffers[m_currentFrame].update(&ubo, sizeof(ubo));
#endif

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
            m_spec.window.shouldResize()) {
            m_spec.window.setShouldResize(false);
            m_swapchain.recreate(m_framebuffers, m_renderPass);
        } else if (result != vk::Result::eSuboptimalKHR) {
            ENSURE(false);
        }
    }

    m_currentFrame = (m_currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

void Renderer::waitIdle() const {
    m_logicalDevice.as<vk::Device>().waitIdle();
}

} // namespace R3

#endif // R3_VULKAN

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
        .physicalDevice = &m_physicalDevice,
        .logicalDevice = &m_logicalDevice,
        .swapchain = &m_swapchain,
    });

    //--- DescriptorPool
    m_descriptorPool = DescriptorPool({
        .logicalDevice = &m_logicalDevice,
        .descriptorSetCount = MAX_FRAMES_IN_FLIGHT,
    });

    //--- Graphics Pipeline
    m_graphicsPipeline = GraphicsPipeline({
        .logicalDevice = &m_logicalDevice,
        .swapchain = &m_swapchain,
        .renderPass = &m_renderPass,
        .descriptorSetLayout = &m_descriptorPool.layout(),
        .vertexShaderPath = "spirv/test.vert.spv",
        .fragmentShaderPath = "spirv/test.frag.spv",
    });

    //--- DepthBuffer
    m_depthBuffer = DepthBuffer({
        .physicalDevice = &m_physicalDevice,
        .logicalDevice = &m_logicalDevice,
        .swapchain = &m_swapchain,
    });

    //--- Framebuffers
    for (const auto& swapchainImageView : m_swapchain.imageViews()) {
        m_framebuffers.emplace_back(FramebufferSpecification{
            .logicalDevice = &m_logicalDevice,
            .swapchain = &m_swapchain,
            .swapchainImageView = &swapchainImageView,
            .depthBufferImageView = &m_depthBuffer.imageView(),
            .renderPass = &m_renderPass,
        });
    }

    //--- CommandPool and CommandBuffers
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
        m_imageAvailable[i] = Semaphore({&m_logicalDevice});
        m_renderFinished[i] = Semaphore({&m_logicalDevice});
        m_inFlight[i] = Fence({&m_logicalDevice});
    }

    //--- Uniforms
    m_uniformBuffers.resize(MAX_FRAMES_IN_FLIGHT);
    for (auto& uniformBuffer : m_uniformBuffers) {
        uniformBuffer = UniformBuffer({
            .physicalDevice = &m_physicalDevice,
            .logicalDevice = &m_logicalDevice,
            .bufferSize = sizeof(UniformBufferObject),
        });
    }

    //--- Update descriptorSets with uniform and texture info
    auto& descriptorSets = m_descriptorPool.descriptorSets();
    for (uint32 i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        UniformDescriptor uniformDescriptors[] = {{
            .uniform = m_uniformBuffers[i],
            .binding = 0,
        }};
        descriptorSets[i].bindResources({uniformDescriptors, {}});
    }
}

void Renderer::render(double dt) {
    const vk::Fence inFlight = m_inFlight[m_currentFrame].as<vk::Fence>();
    auto r = m_logicalDevice.as<vk::Device>().waitForFences(inFlight, vk::False, UINT64_MAX);
    CHECK(r == vk::Result::eSuccess);

    const vk::Semaphore imageAvailable = m_imageAvailable[m_currentFrame].as<vk::Semaphore>();
    auto [result, imageIndex] = m_logicalDevice.as<vk::Device>().acquireNextImageKHR(
        m_swapchain.as<vk::SwapchainKHR>(), UINT64_MAX, imageAvailable);

    [[unlikely]] if (result != vk::Result::eSuccess) {
        if (result == vk::Result::eErrorOutOfDateKHR) {
            resize({});
            return;
        } else if (result != vk::Result::eSuboptimalKHR) {
            LOG(Error, "vulkan error code:", (VkResult)result);
        }
    }

    m_logicalDevice.as<vk::Device>().resetFences(inFlight);

    const CommandBuffer& commandBuffer = m_commandPool.commandBuffers()[m_currentFrame];
    DescriptorSet& descriptorSet = m_descriptorPool.descriptorSets()[m_currentFrame];

    static std::vector<TextureDescriptor> textureCache;

    commandBuffer.resetCommandBuffer();
    commandBuffer.beginCommandBuffer();
    commandBuffer.beginRenderPass(m_renderPass, m_framebuffers[imageIndex]);
    {
        commandBuffer.bindPipeline(m_graphicsPipeline);

        for (const auto& mesh : _Model.meshes()) {
            textureCache.clear();
            for (uint32 textureIndex : mesh.textureIndices()) {
                textureCache.push_back({.texture = _Model.textures()[textureIndex]});
            }
            descriptorSet.bindResources({{}, {textureCache}});
            commandBuffer.bindDescriptorSet(m_graphicsPipeline.layout(), descriptorSet);

            commandBuffer.bindVertexBuffer(mesh.vertexBuffer());
            commandBuffer.bindIndexBuffer(mesh.indexBuffer());
            commandBuffer.as<vk::CommandBuffer>().drawIndexed(mesh.indexCount(), 1, 0, 0, 0);
        }
    }
    commandBuffer.endRenderPass();
    commandBuffer.endCommandBuffer();

#if 1
    static double t = 0.0;
    t += dt;
    UniformBufferObject ubo{};
    ubo.model = glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    ubo.model = glm::rotate(ubo.model, (float)(t * 2.0), glm::vec3(0.0f, 1.0f, 0.0f));
    ubo.view = glm::lookAt(glm::vec3(0.0f, 2.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    ubo.projection = glm::perspective(glm::radians(45.0f), m_spec.window.aspectRatio(), 0.1f, 100.0f);
    m_uniformBuffers[m_currentFrame].update(&ubo, sizeof(ubo));
#endif

    const vk::Semaphore waitSemaphores[] = {m_imageAvailable[m_currentFrame].as<vk::Semaphore>()};
    const vk::Semaphore singalSemaphores[] = {m_renderFinished[m_currentFrame].as<vk::Semaphore>()};
    constexpr vk::PipelineStageFlags waitStages[] = {vk::PipelineStageFlagBits::eColorAttachmentOutput};
    const vk::SwapchainKHR swapchains[]{m_swapchain.as<vk::SwapchainKHR>()};
    const vk::CommandBuffer commandBuffers[] = {commandBuffer.as<vk::CommandBuffer>()};

    const vk::SubmitInfo submitInfo = {
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
    m_logicalDevice.graphicsQueue().as<vk::Queue>().submit(submitInfo, inFlight);

    const vk::PresentInfoKHR presentInfo = {
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
    } catch (std::exception& e) {
        if (result == vk::Result::eErrorOutOfDateKHR || result == vk::Result::eSuboptimalKHR) {
            resize({});
        } else {
            LOG(Error, e.what());
        }
    }

    m_currentFrame = (imageIndex + 1) % MAX_FRAMES_IN_FLIGHT;
}

void Renderer::resize(uvec2 dimensions) {
    m_swapchain.recreate({
        .framebuffers = m_framebuffers,
        .depthBuffer = m_depthBuffer,
        .renderPass = m_renderPass,
        .width = dimensions.x,
        .height = dimensions.y,
    });
}

void Renderer::waitIdle() const {
    m_logicalDevice.as<vk::Device>().waitIdle();
}

} // namespace R3

#endif // R3_VULKAN

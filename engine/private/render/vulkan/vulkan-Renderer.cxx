#if R3_VULKAN

#include "render/Renderer.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vulkan/vulkan.hpp>
#include "api/Check.hpp"
#include "components/ModelComponent.hpp"
#include "core/Entity.hpp"
#include "core/Scene.hpp"
#include "render/ResourceManager.hxx"
#include "ui/UserInterface.hxx"
#include "vulkan-PushConstant.hxx"
#include "vulkan-UniformBufferObject.hxx"

namespace R3 {

Renderer::Renderer(const RendererSpecification& spec)
    : m_window(spec.window) {
    //--- Instance Extensions
    std::vector<const char*> extensions(Instance::queryRequiredExtensions());
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
        .instance = m_instance,
        .window = m_window,
    });

    //--- Physical Device
    m_physicalDevice = PhysicalDevice({
        .instance = m_instance,
        .surface = m_surface,
        .extensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME},
    });

    //--- Logical Device and Queues
    m_logicalDevice = LogicalDevice({
        .instance = m_instance,
        .surface = m_surface,
        .physicalDevice = m_physicalDevice,
    });

    //--- Swapchain
    m_swapchain = Swapchain({
        .physicalDevice = m_physicalDevice,
        .surface = m_surface,
        .logicalDevice = m_logicalDevice,
        .window = m_window,
    });

    //--- RenderPass
    m_renderPass = RenderPass({
        .physicalDevice = m_physicalDevice,
        .logicalDevice = m_logicalDevice,
        .swapchain = m_swapchain,
    });

    //--- ColorBuffer
    m_colorBuffer = ColorBuffer({
        .physicalDevice = m_physicalDevice,
        .logicalDevice = m_logicalDevice,
        .swapchain = m_swapchain,
    });

    //--- DepthBuffer
    m_depthBuffer = DepthBuffer({
        .physicalDevice = m_physicalDevice,
        .logicalDevice = m_logicalDevice,
        .swapchain = m_swapchain,
    });

    //--- Framebuffers
    for (const auto& swapchainImageView : m_swapchain.imageViews()) {
        m_framebuffers.emplace_back(FramebufferSpecification{
            .logicalDevice = m_logicalDevice,
            .swapchain = m_swapchain,
            .swapchainImageView = swapchainImageView,
            .colorBufferImageView = m_colorBuffer.imageView(),
            .depthBufferImageView = m_depthBuffer.imageView(),
            .renderPass = m_renderPass,
        });
    }

    //--- CommandPool and CommandBuffers
    m_commandPool = CommandPool({
        .logicalDevice = m_logicalDevice,
        .swapchain = m_swapchain,
        .type = CommandPoolType::Reset,
        .commandBufferCount = MAX_FRAMES_IN_FLIGHT,
    });

    m_commandPoolLocal = CommandPool({
        .logicalDevice = m_logicalDevice,
        .swapchain = m_swapchain,
        .type = CommandPoolType::Reset,
        .commandBufferCount = 1,
    });

    //--- Synchronization
    for (uint32 i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        m_imageAvailable[i] = Semaphore({m_logicalDevice});
        m_renderFinished[i] = Semaphore({m_logicalDevice});
        m_inFlight[i] = Fence({m_logicalDevice});
    }

    //--- Model Loader
    m_modelLoader = ModelLoader({
        .physicalDevice = m_physicalDevice,
        .logicalDevice = m_logicalDevice,
        .swapchain = m_swapchain,
        .renderPass = m_renderPass,
        .commandPool = m_commandPool,
    });
}

void Renderer::render() {
    UserInterface::newFrame();

    Fence& inFlight = m_inFlight[m_currentFrame];

    vk::Result r = m_logicalDevice.as<vk::Device>().waitForFences(inFlight.as<vk::Fence>(), vk::False, ~0ULL);
    CHECK(r == vk::Result::eSuccess);

    Semaphore& imageAvailable = m_imageAvailable[m_currentFrame];

    auto [result, imageIndex] = m_logicalDevice.as<vk::Device>().acquireNextImageKHR(
        m_swapchain.as<vk::SwapchainKHR>(), ~0ULL, imageAvailable.as<vk::Semaphore>());

    [[unlikely]] if (result != vk::Result::eSuccess) {
        if (result == vk::Result::eErrorOutOfDateKHR) {
            resize();
            return;
        } else if (result != vk::Result::eSuboptimalKHR) {
            LOG(Error, "vulkan error code:", (VkResult)result);
        }
    }

    inFlight.reset();

    const CommandBuffer& commandBuffer = m_commandPool.commandBuffers()[m_currentFrame];

    commandBuffer.resetCommandBuffer();
    commandBuffer.beginCommandBuffer();
    commandBuffer.beginRenderPass(m_renderPass, m_framebuffers[imageIndex]);
    {
        Scene::componentForEach([&](TransformComponent& transform, ModelComponent& model) {
            for (Mesh& mesh : model.meshes()) {
                auto& pipeline = GlobalResourceManager.getGraphicsPipelineById(mesh.pipeline);
                auto& uniform = GlobalResourceManager.getUniformById(mesh.material.uniforms[m_currentFrame]);
                auto& lightUniform = GlobalResourceManager.getUniformById(mesh.material.uniforms[m_currentFrame + 3]);
                auto& descriptorPool = GlobalResourceManager.getDescriptorPoolById(mesh.material.descriptorPool);
                auto& vertexBuffer = GlobalResourceManager.getVertexBufferById(mesh.vertexBuffer);
                auto& indexBuffer = GlobalResourceManager.getIndexBufferById(mesh.indexBuffer);

                commandBuffer.bindPipeline(pipeline);
                commandBuffer.bindDescriptorSet(pipeline.layout(), descriptorPool.descriptorSets()[m_currentFrame]);

                vulkan::LightBufferObject lbo = {
                    .cameraPosition = Scene::cameraPosition(),
                    .pointLights =
                        {
                            vulkan::PointLight{
                                .position = vec3(0.5f, 0.8f, 0.0f),
                                .color = vec3(1.0f, 0, 0),
                                .intensity = vec3(0.2f),
                            },
                        },
                    .lightCount = 1,
                    .pbrFlags = mesh.material.pbrFlags,
                };
                lightUniform.update(&lbo, sizeof(lbo), 0);
                uniform.update(&transform, sizeof(transform), 0);

                commandBuffer.as<vk::CommandBuffer>().pushConstants(pipeline.layout().as<vk::PipelineLayout>(),
                                                                    vk::ShaderStageFlagBits::eVertex,
                                                                    0,
                                                                    sizeof(ViewProjection),
                                                                    &m_viewProjection);

                commandBuffer.bindVertexBuffer(vertexBuffer);
                commandBuffer.bindIndexBuffer(indexBuffer);
                commandBuffer.as<vk::CommandBuffer>().drawIndexed(indexBuffer.count(), 1, 0, 0, 0);
            }
        });

        UserInterface::draw(commandBuffer);
    }
    commandBuffer.endRenderPass();
    commandBuffer.endCommandBuffer();

    const NativeRenderObject imageReady[] = {imageAvailable.handle()};
    NativeRenderObject renderFinished[] = {m_renderFinished[m_currentFrame].handle()};

    const CommandBufferSumbitSpecification commandBufferSumbitSpecification = {
        .waitStages = PipelineStage::ColorAttachmentOutput,
        .waitSemaphores = imageReady,
        .signalSemaphores = renderFinished,
        .fence = &inFlight,
    };
    commandBuffer.submit(commandBufferSumbitSpecification);

    const CommandBufferPresentSpecification commandBufferPresentSpecification = {
        .waitSemaphores = renderFinished,
        .currentImageIndex = imageIndex,
    };

    try {
        result = vk::Result(commandBuffer.present(commandBufferPresentSpecification));
    } catch (std::exception& e) {
        if (result == vk::Result::eErrorOutOfDateKHR || result == vk::Result::eSuboptimalKHR ||
            m_window.shouldResize()) {
            resize();
            m_window.setShouldResize(false);
        } else {
            LOG(Error, e.what());
        }
    }

    m_currentFrame = (m_currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

void Renderer::resize() {
    m_swapchain.recreate({
        .framebuffers = m_framebuffers,
        .colorBuffer = m_colorBuffer,
        .depthBuffer = m_depthBuffer,
        .renderPass = m_renderPass,
    });
}

void Renderer::waitIdle() const {
    m_logicalDevice.as<vk::Device>().waitIdle();
}

} // namespace R3

#endif // R3_VULKAN

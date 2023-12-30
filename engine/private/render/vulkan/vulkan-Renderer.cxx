#if R3_VULKAN

#include "render/Renderer.hxx"

#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_vulkan.h>
#include <imgui.h>
#include <R3>
#include <R3_components>
#include <R3_core>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vulkan/vulkan.hpp>
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

    initializeUserInterface();
}

Renderer::~Renderer() {
    destroyUserInterface();
}

void Renderer::render() {
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
        Scene::componentView<TransformComponent, ModelComponent>().each(
            [&](TransformComponent& transform, ModelComponent& model) {
                for (Mesh& mesh : model.meshes()) {
                    auto* resourceManager = reinterpret_cast<ResourceManager*>(CurrentScene->resourceManager);

                    auto& pipeline = resourceManager->getGraphicsPipelineById(mesh.pipeline);
                    auto& uniform = resourceManager->getUniformById(mesh.material.uniforms[m_currentFrame]);
                    auto& lightUniform = resourceManager->getUniformById(mesh.material.uniforms[m_currentFrame + 3]);
                    auto& descriptorPool = resourceManager->getDescriptorPoolById(mesh.material.descriptorPool);
                    auto& vertexBuffer = resourceManager->getVertexBufferById(mesh.vertexBuffer);
                    auto& indexBuffer = resourceManager->getIndexBufferById(mesh.indexBuffer);

                    commandBuffer.bindPipeline(pipeline);
                    commandBuffer.bindDescriptorSet(pipeline.layout(), descriptorPool.descriptorSets()[m_currentFrame]);

                    vulkan::LightBufferObject lbo = {
                        .cameraPosition = Scene::cameraPosition(),
                        .lightCount = 1,
                        .pbrFlags = mesh.material.pbrFlags,
                        .pointLights =
                            {
                                vulkan::PointLight{
                                    .position = vec3(1.0f, 0.3f, 1.0f),
                                    .color = vec3(0.83f, 0.1f, 0.1f),
                                    .intensity = vec3(0.3f),
                                },
                            },
                    };

                    uniform.update(&transform, sizeof(transform), 0);
                    lightUniform.update(&lbo, sizeof(lbo), 0);

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

        UserInterface::beginFrame();
        UserInterface::populate();
        UserInterface::endFrame();
        UserInterface::drawFrame(commandBuffer);
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

void Renderer::initializeUserInterface() {
    vk::DescriptorPoolSize poolSizes[] = {
        {vk::DescriptorType::eSampler, 1000},
        {vk::DescriptorType::eCombinedImageSampler, 1000},
        {vk::DescriptorType::eSampledImage, 1000},
        {vk::DescriptorType::eStorageImage, 1000},
        {vk::DescriptorType::eUniformTexelBuffer, 1000},
        {vk::DescriptorType::eStorageTexelBuffer, 1000},
        {vk::DescriptorType::eUniformBuffer, 1000},
        {vk::DescriptorType::eStorageBuffer, 1000},
        {vk::DescriptorType::eUniformBufferDynamic, 1000},
        {vk::DescriptorType::eStorageBufferDynamic, 1000},
        {vk::DescriptorType::eInputAttachment, 1000},
    };

    vk::DescriptorPoolCreateInfo descriptorPoolInfo = {
        .sType = vk::StructureType::eDescriptorPoolCreateInfo,
        .pNext = nullptr,
        .flags = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet,
        .maxSets = 1000,
        .poolSizeCount = (uint32)std::size(poolSizes),
        .pPoolSizes = poolSizes,
    };

    m_uiDescriptorPool = Ref<void>(m_logicalDevice.as<vk::Device>().createDescriptorPool(descriptorPoolInfo));

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForVulkan(m_window.handle<GLFWwindow*>(), true);

    ImGui_ImplVulkan_InitInfo initInfo = {
        .Instance = m_instance.as<vk::Instance>(),
        .PhysicalDevice = m_physicalDevice.as<vk::PhysicalDevice>(),
        .Device = m_logicalDevice.as<vk::Device>(),
        .Queue = m_logicalDevice.graphicsQueue().as<vk::Queue>(),
        .DescriptorPool = (VkDescriptorPool)m_uiDescriptorPool.get(),
        .MinImageCount = MAX_FRAMES_IN_FLIGHT,
        .ImageCount = MAX_FRAMES_IN_FLIGHT,
        .MSAASamples = (VkSampleCountFlagBits)m_physicalDevice.sampleCount(),
    };

    ImGui_ImplVulkan_Init(&initInfo, m_renderPass.as<vk::RenderPass>());

    auto& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.Fonts->AddFontFromFileTTF("fonts/Cascadia/CascadiaCode.ttf", 16.0f);
    io.Fonts->Build();

    ImGui_ImplVulkan_CreateFontsTexture();
}

void Renderer::destroyUserInterface() {
    ImGui_ImplVulkan_DestroyFontsTexture();
    m_logicalDevice.as<vk::Device>().destroyDescriptorPool((VkDescriptorPool)m_uiDescriptorPool.get());
    ImGui_ImplVulkan_Shutdown();
}

void Renderer::waitIdle() const {
    m_logicalDevice.as<vk::Device>().waitIdle();
}

} // namespace R3

#endif // R3_VULKAN

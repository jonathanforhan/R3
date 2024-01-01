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
#include "render/ShaderObjects.hxx"

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

    //--- Editor
    m_editor = editor::Editor({
        .window = m_window,
        .instance = m_instance,
        .physicalDevice = m_physicalDevice,
        .logicalDevice = m_logicalDevice,
        .renderPass = m_renderPass,
    });

    //--- Model Loader
    m_modelLoader = ModelLoader({
        .physicalDevice = m_physicalDevice,
        .logicalDevice = m_logicalDevice,
        .swapchain = m_swapchain,
        .renderPass = m_renderPass,
        .commandPool = m_commandPool,
    });

    //--- Shader View Projection
    m_viewProjection = {
        .view = mat4(1.0f),
        .projection = mat4(1.0f),
    };
}

void Renderer::render() {
    Fence& inFlight = m_inFlight[m_currentFrame];

    vk::Result r = m_logicalDevice.as<vk::Device>().waitForFences(inFlight.as<vk::Fence>(), vk::False, ~0ULL);
    CHECK(r == vk::Result::eSuccess);

    Semaphore& imageAvailable = m_imageAvailable[m_currentFrame];

    auto&& [result, imageIndex] = m_logicalDevice.as<vk::Device>().acquireNextImageKHR(
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

    //****************************************** SETUP BEGIN ******************************************//

    updateLighting();

    //******************************************* SETUP END *******************************************//

    const CommandBuffer& commandBuffer = m_commandPool.commandBuffers()[m_currentFrame];
    commandBuffer.resetCommandBuffer();
    commandBuffer.beginCommandBuffer();
    commandBuffer.beginRenderPass(m_renderPass, m_framebuffers[imageIndex]);
    //*************************************** RENDER PASS BEGIN ***************************************//

    // draw every mesh of every model
    auto draw = [&](const TransformComponent& transform, const ModelComponent& model) {
        for (const Mesh& mesh : model.meshes()) {
            auto* resourceManager = reinterpret_cast<ResourceManager*>(CurrentScene->resourceManager);

            auto& pipeline = resourceManager->getGraphicsPipelineById(mesh.pipeline);
            auto& uniform = resourceManager->getUniformById(mesh.material.uniforms[m_currentFrame]);
            auto& lightUniform = resourceManager->getUniformById(mesh.material.uniforms[m_currentFrame + 3]);
            auto& descriptorPool = resourceManager->getDescriptorPoolById(mesh.material.descriptorPool);
            auto& vertexBuffer = resourceManager->getVertexBufferById(mesh.vertexBuffer);
            auto& indexBuffer = resourceManager->getIndexBufferById(mesh.indexBuffer);

            commandBuffer.bindPipeline(pipeline);
            commandBuffer.bindDescriptorSet(pipeline.layout(), descriptorPool.descriptorSets()[m_currentFrame]);

            commandBuffer.pushConstants(
                pipeline.layout(), ShaderStage::Vertex, &m_viewProjection, sizeof(m_viewProjection));
            uniform.update(&transform, sizeof(transform));
            FragmentUniformBufferObject fubo = {
                .cameraPosition = Scene::cameraPosition(),
                .pbrFlags = mesh.material.pbrFlags,
                .lightCount = static_cast<uint32>(m_pointLights.size()),
            };
            std::copy(m_pointLights.begin(), m_pointLights.end(), fubo.pointLights);
            lightUniform.update(&fubo, sizeof(fubo));

            commandBuffer.bindVertexBuffer(vertexBuffer);
            commandBuffer.bindIndexBuffer(indexBuffer);
            commandBuffer.as<vk::CommandBuffer>().drawIndexed(indexBuffer.count(), 1, 0, 0, 0);
        }
    };
    Entity::componentView<TransformComponent, ModelComponent>().each(draw);
    m_editor.drawFrame(commandBuffer);

    //**************************************** RENDER PASS END ****************************************//
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

void Renderer::renderEditorInterface(double dt) {
    m_editor.beginFrame();
    m_editor.initializeDocking();
    m_editor.displayHierarchy();
    m_editor.displayProperties();
    m_editor.displayDeltaTime(dt);
    m_editor.endFrame();
}

void Renderer::updateLighting() {
    m_pointLights.clear();

    Entity::componentView<LightComponent>().each([&, this](LightComponent& light) {
        if (m_pointLights.size() < MAX_LIGHTS) {
            m_pointLights.emplace_back(PointLightShaderObject{
                .position = light.position,
                .color = light.color,
                .intensity = light.intensity,
            });
        }
    });
}

} // namespace R3

#endif // R3_VULKAN

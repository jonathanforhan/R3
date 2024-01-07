#if R3_VULKAN

#include "render/Renderer.hpp"

#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_vulkan.h>
#include <imgui.h>
#include <R3>
#include <R3_core>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vulkan/vulkan.hpp>
#include "components/LightComponent.hpp"
#include "input/MouseEvent.hpp"
#include "input/WindowEvent.hpp"
#include "render/ResourceManager.hxx"
#include "render/ShaderObjects.hpp"

namespace R3 {

static constexpr auto DEPTH_ARRAY_SCALE = 2048;

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
        .colorAttachment =
            {
                .format = m_swapchain.surfaceFormat(),
                .sampleCount = m_physicalDevice.sampleCount(),
            },
        .depthAttachment =
            {
                .sampleCount = m_physicalDevice.sampleCount(),
            },
    });

    //--- ColorBuffer
    m_colorBuffer = ColorBuffer({
        .physicalDevice = m_physicalDevice,
        .logicalDevice = m_logicalDevice,
        .format = m_swapchain.surfaceFormat(),
        .extent = m_swapchain.extent(),
        .sampleCount = m_physicalDevice.sampleCount(),
    });

    //--- DepthBuffer
    m_depthBuffer = DepthBuffer({
        .physicalDevice = m_physicalDevice,
        .logicalDevice = m_logicalDevice,
        .extent = m_swapchain.extent(),
        .sampleCount = m_physicalDevice.sampleCount(),
    });

    //--- Framebuffers
    for (const auto& swapchainImageView : m_swapchain.imageViews()) {
        // NOTE this format [ COLOR, DEPTH, SWAPCHAIN_IMAGE ]
        // is important. It is mirrored in the layout of the renderpass attachments
        const ImageView* attachments[] = {
            &m_colorBuffer.imageView(),
            &m_depthBuffer.imageView(),
            &swapchainImageView,
        };
        m_framebuffers.emplace_back(FramebufferSpecification{
            .logicalDevice = m_logicalDevice,
            .renderPass = m_renderPass,
            .attachments = attachments,
            .extent = m_swapchain.extent(),
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
        .storageBuffer = m_storageBuffer,
    });

    //--- Shader View Projection
    m_viewProjection = {
        .view = mat4(1.0f),
        .projection = mat4(1.0f),
    };

    //--- StorageBuffer
    m_storageBuffer = StorageBuffer({
        .physicalDevice = m_physicalDevice,
        .logicalDevice = m_logicalDevice,
        .bufferSize = sizeof(uint32) * DEPTH_ARRAY_SCALE,
    });
}

void Renderer::preLoop() {
    using MouseReleaseCallback = std::function<void(const MouseButtonReleaseEvent&)>;
    using MousePressCallback = std::function<void(const MouseButtonPressEvent&)>;
    using WindowContentScaleCallback = std::function<void(const WindowContentScaleChangeEvent&)>;
    using WindowResizeCallback = std::function<void(const WindowResizeEvent&)>;

    static vec2 prevMousePosition = m_cursorPosition;

    // Save position on mouse press for comparison
    const MousePressCallback setMousePosition = [this](const auto&) { prevMousePosition = m_cursorPosition; };
    Scene::bindEventListener(setMousePosition);

    // Change editor selected entity if pressed and not dragging the view
    const MouseReleaseCallback getSelectedEntity = [this](const MouseButtonReleaseEvent& e) {
        float localityX = glm::abs(prevMousePosition.x - m_cursorPosition.x);
        float localityY = glm::abs(prevMousePosition.y - m_cursorPosition.y);

        if (e.payload.button == MouseButton::Left && (localityY + localityX) < 5.0f) {
            m_editor.setCurrentEntity(getHoveredEntity());
        }
    };
    Scene::bindEventListener(getSelectedEntity);

    // Rescale the font and window size for High DPI
    float prevScaleInit = (m_window.contentScale().x + m_window.contentScale().y) / 2.0f;
    const WindowContentScaleCallback scaleCallback = [=, this](const WindowContentScaleChangeEvent& e) {
        static float prevScale = prevScaleInit;
        float scale = (e.payload.scaleX + e.payload.scaleY) / 2.0f;
        m_editor.setContentScale(scale);

        float deltaScale = scale / prevScale;
        ivec2 windowSize = m_window.size();
        ivec2 newSize(windowSize.x * deltaScale, windowSize.y * deltaScale);
        m_window.resize(newSize);

        ivec2 windowPosition = m_window.position();
        windowPosition.x += (windowSize.x - newSize.x) / 4;
        m_window.setPosition(windowPosition);

        prevScale = scale;
    };
    Scene::bindEventListener(scaleCallback);

    // Rescale on window resizes as well, glfw bug
    const WindowResizeCallback resizeCallback = [this](const WindowResizeEvent&) {
        vec2 scaleXY = m_window.contentScale();
        float scale = (scaleXY.x + scaleXY.y) / 2.0f;
        m_editor.setContentScale(scale);
    };
    Scene::bindEventListener(resizeCallback);
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
    constexpr uint32 zeros[DEPTH_ARRAY_SCALE] = {};
    m_storageBuffer.write(zeros, sizeof(zeros), 0);

    //******************************************* SETUP END *******************************************//

    const CommandBuffer& cmd = m_commandPool.commandBuffers()[m_currentFrame];
    cmd.resetCommandBuffer();
    cmd.beginCommandBuffer();
    cmd.beginRenderPass(m_renderPass, m_framebuffers[imageIndex]);
    //*************************************** RENDER PASS BEGIN ***************************************//

    // draw every mesh of every model
    auto draw = [&](auto entity, const TransformComponent& transform, const ModelComponent& model) {
        for (const Mesh& mesh : model.meshes) {
            auto* resourceManager = static_cast<ResourceManager*>(CurrentScene->resourceManager);

            auto& pipeline = resourceManager->getGraphicsPipelineById(mesh.pipeline);
            auto& uniform = resourceManager->getUniformById(mesh.material.uniforms[m_currentFrame]);
            auto& lightUniform = resourceManager->getUniformById(mesh.material.uniforms[m_currentFrame + 3]);
            auto& descriptorPool = resourceManager->getDescriptorPoolById(mesh.material.descriptorPool);
            auto& vertexBuffer = resourceManager->getVertexBufferById(mesh.vertexBuffer);
            auto& indexBuffer = resourceManager->getIndexBufferById(mesh.indexBuffer);

            cmd.bindPipeline(pipeline);
            cmd.bindDescriptorSet(pipeline.layout(), descriptorPool.descriptorSets()[m_currentFrame]);

            FragmentPushConstant fragmentPushConstant = {
                .cursorPosition = m_cursorPosition,
                .uid = uint32(entity),
                .selected = m_editor.currentEntity(),
            };
            cmd.pushConstants(
                pipeline.layout(), ShaderStage::Fragment, &fragmentPushConstant, sizeof(fragmentPushConstant));

            VertexUniformBufferObject vubo = {
                .model = transform,
                .view = m_viewProjection.view,
                .projection = m_viewProjection.projection,
            };
            for (usize i = 0; i < model.skeleton.finalJointsMatrices.size(); i++) {
                vubo.finalBoneTransforms[i] = model.skeleton.finalJointsMatrices[i];
            }
            uniform.write(&vubo, sizeof(vubo));

            FragmentUniformBufferObject fubo = {
                .cameraPosition = Scene::cameraPosition(),
                .pbrFlags = mesh.material.pbrFlags,
                .lightCount = static_cast<uint32>(m_pointLights.size()),
            };
            std::copy(m_pointLights.begin(), m_pointLights.end(), fubo.pointLights);
            lightUniform.write(&fubo, sizeof(fubo));

            cmd.bindVertexBuffer(vertexBuffer);
            cmd.bindIndexBuffer(indexBuffer);
            cmd.as<vk::CommandBuffer>().drawIndexed(indexBuffer.count(), 1, 0, 0, 0);
        }
    };
    Entity::componentView<TransformComponent, ModelComponent>().each(draw);
    m_editor.drawFrame(cmd);

    //**************************************** RENDER PASS END ****************************************//
    cmd.endRenderPass();
    cmd.endCommandBuffer();

    const NativeRenderObject imageReady[] = {imageAvailable.handle()};
    NativeRenderObject renderFinished[] = {m_renderFinished[m_currentFrame].handle()};

    const CommandBufferSumbitSpecification commandBufferSumbitSpecification = {
        .waitStages = PipelineStage::ColorAttachmentOutput,
        .waitSemaphores = imageReady,
        .signalSemaphores = renderFinished,
        .fence = &inFlight,
    };
    cmd.submit(commandBufferSumbitSpecification);

    const CommandBufferPresentSpecification commandBufferPresentSpecification = {
        .waitSemaphores = renderFinished,
        .currentImageIndex = imageIndex,
    };

    try {
        result = vk::Result(cmd.present(commandBufferPresentSpecification));
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

void Renderer::recreate() {
    resize();
}

uuid32 Renderer::getHoveredEntity() const {
    auto* buf = m_storageBuffer.read<uint32>();

    for (auto i = 0; i < DEPTH_ARRAY_SCALE; i++) {
        if (buf[i] != 0) {
            return buf[i];
        }
    }

    return undefined;
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

void Renderer::waitIdle() const {
    m_logicalDevice.as<vk::Device>().waitIdle();
}

} // namespace R3

#endif // R3_VULKAN

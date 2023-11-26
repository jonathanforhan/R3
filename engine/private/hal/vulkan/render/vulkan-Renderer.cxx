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

Renderer::Renderer(RenderSpecification spec) {
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

    //--- Graphics Pipeline
}

Renderer::~Renderer() {
    m_swapchain.destroy();
    m_logicalDevice.destroy();
    m_surface.destroy();
    m_instance.destroy();
}

} // namespace R3

#endif // R3_VULKAN

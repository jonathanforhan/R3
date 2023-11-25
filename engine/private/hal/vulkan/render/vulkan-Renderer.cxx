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

Renderer::Renderer() {
    //--- Extensions
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

    //--- Physical Device
    m_physicalDevice.select({
        .instance = &m_instance,
    });
}

Renderer::~Renderer() {
    m_instance.destroy();
}

} // namespace R3

#endif // R3_VULKAN

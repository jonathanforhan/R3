#if R3_VULKAN

#include "render/Instance.hpp"

// clang-format off
#include <vulkan/vulkan.hpp>
#include <GLFW/glfw3.h>
// clang-format on
#include "api/Check.hpp"
#include "api/Log.hpp"
#include "api/Version.hpp"

#define R3_VK_VERSION (VK_MAKE_API_VERSION(0, VULKAN_VERSION_MAJOR, VULKAN_VERSION_MINOR, 0))

namespace R3 {

namespace local {

#if R3_VALIDATION_LAYERS_ENABLED
static VKAPI_ATTR VkBool32 VKAPI_CALL validationDebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                                              VkDebugUtilsMessageTypeFlagsEXT messageType,
                                                              const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                                                              void* pUserData) {
    (void)messageType;
    (void)pCallbackData;
    (void)pUserData;

    switch (messageSeverity) {
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
            LOG(Verbose, pCallbackData->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
            LOG(Info, pCallbackData->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
            LOG(Warning, pCallbackData->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
            LOG(Error, pCallbackData->pMessage);
            break;
        default:
            break;
    }

    return VK_FALSE;
}
#endif // R3_VALIDATION_LAYERS

} // namespace local

Instance::Instance(const InstanceSpecification& spec) {
    const vk::ApplicationInfo applicationInfo = {
        .sType = vk::StructureType::eApplicationInfo,
        .pNext = nullptr,
        .pApplicationName = spec.applicationName.data(),
        .applicationVersion = R3_VK_VERSION,
        .pEngineName = "R3",
        .engineVersion = R3_VK_VERSION,
        .apiVersion = R3_VK_VERSION,
    };

    CHECK(checkExtensionSupport(spec.extensions));
    CHECK(checkValidationLayerSupport(spec.validationLayers));

#if R3_VALIDATION_LAYERS_ENABLED
    const vk::DebugUtilsMessengerCreateInfoEXT debugMessengerCreateInfo = {
        .sType = vk::StructureType::eDebugUtilsMessengerCreateInfoEXT,
        .pNext = nullptr,
        .flags = {},
        .messageSeverity =
            vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose | vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo |
            vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning | vk::DebugUtilsMessageSeverityFlagBitsEXT::eError,
        .messageType =
#if R3_LOG_GENERAL_RENDER_INFO
            vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
#endif
            vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance,
        .pfnUserCallback = local::validationDebugCallback,
        .pUserData = nullptr,
    };

    const vk::InstanceCreateInfo instanceCreateInfo = {
        .sType = vk::StructureType::eInstanceCreateInfo,
        .pNext = &debugMessengerCreateInfo,
        .flags = {},
        .pApplicationInfo = &applicationInfo,
        .enabledLayerCount = static_cast<uint32>(spec.validationLayers.size()),
        .ppEnabledLayerNames = spec.validationLayers.data(),
        .enabledExtensionCount = static_cast<uint32>(spec.extensions.size()),
        .ppEnabledExtensionNames = spec.extensions.data(),
    };
#else
    const vk::InstanceCreateInfo instanceCreateInfo = {
        .sType = vk::StructureType::eInstanceCreateInfo,
        .pNext = nullptr,
        .flags = {},
        .pApplicationInfo = &applicationInfo,
        .enabledLayerCount = 0,
        .ppEnabledLayerNames = nullptr,
        .enabledExtensionCount = static_cast<uint32>(spec.extensions.size()),
        .ppEnabledExtensionNames = spec.extensions.data(),
    };
#endif
    setHandle(vk::createInstance(instanceCreateInfo));
}

Instance::~Instance() {
    if (validHandle()) {
        as<vk::Instance>().destroy();
    }
}

std::vector<const char*> Instance::queryRequiredExtensions() {
    uint32 extensionCount = 0;
    const char** extensions_ = glfwGetRequiredInstanceExtensions(&extensionCount);
    CHECK(extensions_ != nullptr);
    return std::vector<const char*>(extensions_, extensions_ + extensionCount);
}

bool Instance::checkExtensionSupport(std::span<const char*> requiredExtensions) const {
    std::vector<vk::ExtensionProperties> instanceExtensions = vk::enumerateInstanceExtensionProperties();

    for (const auto& requiredExtension : requiredExtensions) {
        auto it = std::ranges::find_if(instanceExtensions, [=](vk::ExtensionProperties& extension) -> bool {
            return strcmp(requiredExtension, extension.extensionName) == 0;
        });

        if (it == instanceExtensions.end()) {
            return false;
        }
    }

    return true;
}

bool Instance::checkValidationLayerSupport(std::span<const char*> requiredValidationLayers) const {
    std::vector<vk::LayerProperties> validationLayers = vk::enumerateInstanceLayerProperties();

    for (const char* requiredValidationLayer : requiredValidationLayers) {
        auto it = std::ranges::find_if(validationLayers, [=](vk::LayerProperties& layer) -> bool {
            return strcmp(requiredValidationLayer, layer.layerName) == 0;
        });

        if (it == validationLayers.end()) {
            return false;
        }
    }

    return true;
}

} // namespace R3

#endif // R3_VULKAN

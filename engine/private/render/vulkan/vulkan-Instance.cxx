#if R3_VULKAN

#include "render/Instance.hpp"

#include <vulkan/vulkan.hpp>
#include <cstring>
#include <vector>
#include "api/Check.hpp"
#include "api/Log.hpp"
#include "api/Types.hpp"
#include "api/Version.hpp"

#define R3_VK_VERSION (VK_MAKE_API_VERSION(0, VULKAN_VERSION_MAJOR, VULKAN_VERSION_MINOR, 0))

static VkDebugUtilsMessengerEXT s_debugMessenger;
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

namespace R3 {

void Instance::create(const InstanceSpecification& spec) {
    m_spec = spec;

    vk::ApplicationInfo applicationInfo = {
        .sType = vk::StructureType::eApplicationInfo,
        .pNext = nullptr,
        .pApplicationName = m_spec.applicationName,
        .applicationVersion = R3_VK_VERSION,
        .pEngineName = "R3",
        .engineVersion = R3_VK_VERSION,
        .apiVersion = R3_VK_VERSION,
    };

    CHECK(checkExtensionSupport(m_spec.extensions));
    CHECK(checkValidationLayerSupport(m_spec.validationLayers));

#if R3_VALIDATION_LAYERS_ENABLED
    vk::DebugUtilsMessengerCreateInfoEXT debugMessengerCreateInfo = {
        .sType = vk::StructureType::eDebugUtilsMessengerCreateInfoEXT,
        .pNext = nullptr,
        .flags = {},
        .messageSeverity =
            vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose | vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo |
            vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning | vk::DebugUtilsMessageSeverityFlagBitsEXT::eError,
        .messageType =
#if R3_LOG_EXTENDED_RENDER_INFO
            vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
#endif
            vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance,
        .pfnUserCallback = validationDebugCallback,
        .pUserData = nullptr,
    };

    vk::InstanceCreateInfo instanceCreateInfo = {
        .sType = vk::StructureType::eInstanceCreateInfo,
        .pNext = &debugMessengerCreateInfo,
        .flags = {},
        .pApplicationInfo = &applicationInfo,
        .enabledLayerCount = static_cast<uint32>(m_spec.validationLayers.size()),
        .ppEnabledLayerNames = m_spec.validationLayers.data(),
        .enabledExtensionCount = static_cast<uint32>(m_spec.extensions.size()),
        .ppEnabledExtensionNames = m_spec.extensions.data(),
    };
#else
    vk::InstanceCreateInfo instanceCreateInfo = {
        .sType = vk::StructureType::eInstanceCreateInfo,
        .pNext = &debugMessengerCreateInfo,
        .flags = {},
        .pApplicationInfo = &applicationInfo,
        .enabledLayerCount = 0,
        .ppEnabledLayerNames = nullptr,
        .enabledExtensionCount = static_cast<uint32>(m_spec.extensions.size()),
        .ppEnabledExtensionNames = m_spec.extensions.data(),
    };
#endif
    setHandle(vk::createInstance(instanceCreateInfo));
}

void Instance::destroy() {
    as<vk::Instance>().destroy();
}

bool Instance::checkExtensionSupport(const std::vector<const char*>& requiredExtensions) const {
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

bool Instance::checkValidationLayerSupport(const std::vector<const char*>& requiredValidationLayers) const {
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

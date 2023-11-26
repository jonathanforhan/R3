#if R3_VULKAN

#include "render/Instance.hpp"

#include <vulkan/vulkan.h>
#include <cstring>
#include <vector>
#include "api/Check.hpp"
#include "api/Ensure.hpp"
#include "api/Log.hpp"
#include "api/Types.hpp"
#include "api/Version.hpp"

#define R3_VK_CALL_EXT(_Ext, _Inst, ...) ((PFN_##_Ext)(vkGetInstanceProcAddr(_Inst, #_Ext)))(_Inst, __VA_ARGS__)
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

    VkApplicationInfo applicationInfo = {
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
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
    VkDebugUtilsMessengerCreateInfoEXT debugMessengerCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
        .pNext = nullptr,
        .flags = 0U,
        .messageSeverity =
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
        .messageType =
#if R3_LOG_EXTENDED_RENDER_INFO
            VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
#endif
            VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
        .pfnUserCallback = validationDebugCallback,
        .pUserData = nullptr,
    };

    VkInstanceCreateInfo instanceCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pNext = &debugMessengerCreateInfo,
        .flags = 0U,
        .pApplicationInfo = &applicationInfo,
        .enabledLayerCount = static_cast<uint32>(m_spec.validationLayers.size()),
        .ppEnabledLayerNames = m_spec.validationLayers.data(),
        .enabledExtensionCount = static_cast<uint32>(m_spec.extensions.size()),
        .ppEnabledExtensionNames = m_spec.extensions.data(),
    };
#else
    VkInstanceCreateInfo instanceCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0U,
        .pApplicationInfo = &applicationInfo,
        .enabledLayerCount = 0,
        .ppEnabledLayerNames = nullptr,
        .enabledExtensionCount = static_cast<uint32>(m_spec.extensions.size()),
        .ppEnabledExtensionNames = m_spec.extensions.data(),
    };
#endif
    ENSURE(vkCreateInstance(&instanceCreateInfo, nullptr, handlePtr<VkInstance*>()) == VK_SUCCESS);

#if R3_VALIDATION_LAYERS_ENABLED
    ENSURE(R3_VK_CALL_EXT(vkCreateDebugUtilsMessengerEXT,
                          handle<VkInstance>(),
                          &debugMessengerCreateInfo,
                          nullptr,
                          &s_debugMessenger) == VK_SUCCESS);
#endif
}

void Instance::destroy() {
#if R3_VALIDATION_LAYERS_ENABLED
    R3_VK_CALL_EXT(vkDestroyDebugUtilsMessengerEXT, handle<VkInstance>(), s_debugMessenger, nullptr);
#endif
    vkDestroyInstance(handle<VkInstance>(), nullptr);
}

bool Instance::checkExtensionSupport(const std::vector<const char*>& requiredExtensions) const {
    uint32 instanceExtensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &instanceExtensionCount, nullptr);
    CHECK(instanceExtensionCount != 0);

    std::vector<VkExtensionProperties> instanceExtensions(instanceExtensionCount);
    vkEnumerateInstanceExtensionProperties(nullptr, &instanceExtensionCount, instanceExtensions.data());

    for (const auto& requiredExtension : requiredExtensions) {
        auto it = std::ranges::find_if(instanceExtensions, [=](VkExtensionProperties& extension) -> bool {
            return strcmp(requiredExtension, extension.extensionName) == 0;
        });

        if (it == instanceExtensions.end()) {
            return false;
        }
    }

    return true;
}

bool Instance::checkValidationLayerSupport(const std::vector<const char*>& requiredValidationLayers) const {
    uint32 validationLayerCount = 0;
    vkEnumerateInstanceLayerProperties(&validationLayerCount, nullptr);
    CHECK(validationLayerCount != 0);

    std::vector<VkLayerProperties> validationLayers(validationLayerCount);
    vkEnumerateInstanceLayerProperties(&validationLayerCount, validationLayers.data());

    for (const char* requiredValidationLayer : requiredValidationLayers) {
        auto it = std::ranges::find_if(validationLayers, [=](VkLayerProperties& layer) -> bool {
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

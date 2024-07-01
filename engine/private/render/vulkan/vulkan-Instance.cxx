// clang-format off
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>
// clang-format on
#include "vulkan-Instance.hxx"
#include <api/Log.hpp>
#include <api/Result.hpp>
#include <api/Types.hpp>
#include <api/Version.hpp>
#include <expected>
#include <iostream>
#include <span>
#include <string_view>
#include <vector>

namespace R3::vulkan {

static constexpr auto VK_VERSION = VK_MAKE_API_VERSION(0, VULKAN_VERSION_MAJOR, VULKAN_VERSION_MINOR, 0);

extern "C" static VKAPI_ATTR VkBool32 VKAPI_CALL
validationDebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                        VkDebugUtilsMessageTypeFlagsEXT messageType,
                        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                        void* pUserData) {
    (void)messageType;
    (void)pCallbackData;
    (void)pUserData;

    switch (messageSeverity) {
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
            std::cerr << pCallbackData->pMessage;
            break;
        default:
            break;
    }

    return VK_FALSE;
}

Result<Instance> Instance::create(const InstanceSpecification& spec) {
    Instance self;

    const VkApplicationInfo applicationInfo = {
        .sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pNext              = nullptr,
        .pApplicationName   = spec.applicationName,
        .applicationVersion = VK_VERSION,
        .pEngineName        = "R3",
        .engineVersion      = VK_VERSION,
        .apiVersion         = VK_VERSION,
    };

    if (!self.checkExtensionSupport(spec.extensions)) {
        R3_LOG(Error, "VkInstance does not support necessary extensions");
        return std::unexpected(Error::UnsupportedFeature);
    }

    if (!self.checkValidationLayerSupport(spec.validationLayers)) {
        R3_LOG(Error, "VkInstance does not support necessary validation layers");
        return std::unexpected(Error::UnsupportedFeature);
    }

    VkInstanceCreateInfo instanceCreateInfo = {
        .sType                   = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pNext                   = nullptr,
        .flags                   = {},
        .pApplicationInfo        = &applicationInfo,
        .enabledLayerCount       = 0,
        .ppEnabledLayerNames     = nullptr,
        .enabledExtensionCount   = static_cast<uint32>(spec.extensions.size()),
        .ppEnabledExtensionNames = spec.extensions.data(),
    };

#if R3_VALIDATION_LAYERS
    const VkDebugUtilsMessengerCreateInfoEXT debugMessengerCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
        .pNext = nullptr,
        .flags = {},
        .messageSeverity =
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
        .messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
        .pfnUserCallback = validationDebugCallback,
        .pUserData       = nullptr,
    };

    instanceCreateInfo.pNext               = &debugMessengerCreateInfo;
    instanceCreateInfo.enabledLayerCount   = static_cast<uint32>(spec.validationLayers.size());
    instanceCreateInfo.ppEnabledLayerNames = spec.validationLayers.data();
#endif

    VkResult result = vkCreateInstance(&instanceCreateInfo, nullptr, &self.m_handle);
    if (result != VK_SUCCESS) {
        R3_LOG(Error, "vkCreateInstance failure {}", (int)result);
        return std::unexpected(Error::InitializationFailure);
    }

    return self;
}

Instance::~Instance() {
    vkDestroyInstance(m_handle, nullptr);
}

std::vector<const char*> Instance::queryRequiredExtensions() {
    uint32 extensionCount   = 0;
    const char** extensions = glfwGetRequiredInstanceExtensions(&extensionCount);
    if (extensions == nullptr) {
        R3_LOG(Error, "glfwGetRequiredInstanceExtensions returned null");
    }
    return std::vector(extensions, extensions + extensionCount);
}

bool Instance::checkExtensionSupport(std::span<const char* const> requiredExtensions) const {
    uint32 extensionCount = 0;
    (void)vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

    std::vector<VkExtensionProperties> instanceExtensions(extensionCount);
    (void)vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, instanceExtensions.data());

    for (const auto& requiredExtension : requiredExtensions) {
        auto it = std::ranges::find_if(instanceExtensions, [=](const VkExtensionProperties& extension) {
            return std::string_view(requiredExtension) == std::string_view(extension.extensionName);
        });

        if (it == instanceExtensions.end()) {
            return false;
        }
    }

    return true;
}

bool Instance::checkValidationLayerSupport(std::span<const char* const> requiredValidationLayers) const {
    uint32 layerCount = 0;
    (void)vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> validationLayers(layerCount);
    (void)vkEnumerateInstanceLayerProperties(&layerCount, validationLayers.data());

    for (const char* requiredValidationLayer : requiredValidationLayers) {
        auto it = std::ranges::find_if(validationLayers, [=](const VkLayerProperties& layer) {
            return std::string_view(requiredValidationLayer) == std::string_view(layer.layerName);
        });

        if (it == validationLayers.end()) {
            return false;
        }
    }

    return true;
}

} // namespace R3::vulkan

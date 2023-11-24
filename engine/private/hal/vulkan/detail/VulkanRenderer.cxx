#if R3_VULKAN

#include "VulkanRenderer.hxx"
#include <GLFW/glfw3.h>
#include <vector>
#include "api/Ensure.hpp"
#include "api/Check.hpp"
#include "api/Log.hpp"
#include "api/Types.hpp"

#define CALL_EXT(_Ext, _Inst, ...) ((PFN_##_Ext)(vkGetInstanceProcAddr(_Inst, #_Ext)))(_Inst, __VA_ARGS__)

static VKAPI_ATTR VkBool32 VKAPI_CALL validationDebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                                              VkDebugUtilsMessageTypeFlagsEXT messageType,
                                                              const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                                                              void* pUserData) {
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

namespace R3::vulkan {

VulkanRenderer::VulkanRenderer()
    : m_instance(VK_NULL_HANDLE), m_physicalDevice(VK_NULL_HANDLE) {

    try {
        createInstance();
#if !R3_BUILD_DIST
        setupDebugMessenger();
#endif // !R3_BUILD_DIST
        choosePhysicalDevice();
    } catch (std::exception& e) {
        LOG(Error, e.what());
        throw e;
    }
}

VulkanRenderer::~VulkanRenderer() {
#if !R3_BUILD_DIST
    CALL_EXT(vkDestroyDebugUtilsMessengerEXT, m_instance, m_debugMessenger, nullptr);
#endif // !R3_BUILD_DIST
    vkDestroyInstance(m_instance, nullptr);
}

void VulkanRenderer::createInstance() {
    VkApplicationInfo applicationInfo = {
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pNext = nullptr,
        .pApplicationName = "R3 Game Engine",
        .applicationVersion = VK_API_VERSION_1_3,
        .pEngineName = "R3",
        .engineVersion = VK_API_VERSION_1_3,
        .apiVersion = VK_API_VERSION_1_3,
    };

    // required GLFW extentions
    uint32 glfwExtensionCount = 0;
    const char** p = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
    std::vector<const char*> glfwExtensions(p, p + glfwExtensionCount);

#if !R3_BUILD_DIST
    glfwExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif // !R3_BUILD_DIST

    CHECK(checkInstanceExtensionSupport(glfwExtensions));

    CHECK(checkValidationLayerSupport());

#if !R3_BUILD_DIST
    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
        .pNext = nullptr,
        .flags = 0U,
        .messageSeverity =
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
        .messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                       VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
        .pfnUserCallback = validationDebugCallback,
        .pUserData = nullptr,
    };

    VkInstanceCreateInfo instanceCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pNext = &debugCreateInfo,
        .flags = 0U,
        .pApplicationInfo = &applicationInfo,
        .enabledLayerCount = sizeof(VALIDATION_LAYERS) / sizeof(*VALIDATION_LAYERS),
        .ppEnabledLayerNames = VALIDATION_LAYERS,
        .enabledExtensionCount = static_cast<uint32>(glfwExtensions.size()),
        .ppEnabledExtensionNames = glfwExtensions.data(),
    };
#else
    VkInstanceCreateInfo instanceCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0U,
        .pApplicationInfo = &applicationInfo,
        .enabledLayerCount = 0,
        .ppEnabledLayerNames = nullptr,
        .enabledExtensionCount = static_cast<uint32>(glfwExtensions.size()),
        .ppEnabledExtensionNames = glfwExtensions.data(),
    };
#endif // !R3_BUILD_DIST

    ENSURE(vkCreateInstance(&instanceCreateInfo, nullptr, &m_instance) == VK_SUCCESS);
}

void VulkanRenderer::choosePhysicalDevice() {
    uint32 physicalDeviceCount = 0;
    vkEnumeratePhysicalDevices(m_instance, &physicalDeviceCount, nullptr);
    CHECK(physicalDeviceCount != 0);

    std::vector<VkPhysicalDevice> physicalDevices(physicalDeviceCount);
    vkEnumeratePhysicalDevices(m_instance, &physicalDeviceCount, physicalDevices.data());

    for (const auto& physicalDevice : physicalDevices) {
        if (checkPhysicalDeviceSuitable(physicalDevice)) {
            m_physicalDevice = physicalDevice;
            break;
        }
    }

    ENSURE(m_physicalDevice != VK_NULL_HANDLE);
}

void VulkanRenderer::setupDebugMessenger() {
#if !R3_BUILD_DIST
    VkDebugUtilsMessengerCreateInfoEXT debugMessengerCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
        .pNext = nullptr,
        .flags = 0U,
        .messageSeverity =
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
        .messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                       VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
        .pfnUserCallback = validationDebugCallback,
        .pUserData = nullptr,
    };

    VkResult result =
        CALL_EXT(vkCreateDebugUtilsMessengerEXT, m_instance, &debugMessengerCreateInfo, nullptr, &m_debugMessenger);
    ENSURE(result == VK_SUCCESS);
#endif // !R3_BUILD_DIST
}

bool VulkanRenderer::checkInstanceExtensionSupport(std::span<const char*> requiredExtensions) const {
    uint32 instanceExtensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &instanceExtensionCount, nullptr);

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

bool VulkanRenderer::checkValidationLayerSupport() const {
#if !R3_BUILD_DIST

    uint32 validationLayerCount = 0;
    vkEnumerateInstanceLayerProperties(&validationLayerCount, nullptr);

    std::vector<VkLayerProperties> validationLayers(validationLayerCount);
    vkEnumerateInstanceLayerProperties(&validationLayerCount, validationLayers.data());

    for (const char* requiredValidationLayer : VALIDATION_LAYERS) {
        auto it = std::ranges::find_if(validationLayers, [=](VkLayerProperties& layer) -> bool {
            return strcmp(requiredValidationLayer, layer.layerName) == 0;
        });

        if (it == validationLayers.end()) {
            return false;
        }
    }

#endif // !R3_BUILD_DIST

    return true;
}

bool VulkanRenderer::checkPhysicalDeviceSuitable(VkPhysicalDevice physicalDevice) const {
    VkPhysicalDeviceProperties physicalDeviceProperties;
    vkGetPhysicalDeviceProperties(physicalDevice, &physicalDeviceProperties);

    VkPhysicalDeviceFeatures physicalDeviceFeatures;
    vkGetPhysicalDeviceFeatures(physicalDevice, &physicalDeviceFeatures);

    if (physicalDeviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
        return true;

    return false;
}

} // namespace R3::vulkan

#endif // R3_VULKAN

#include "vulkan-RenderContext.hpp"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <format>
#include <system_error>
#include <vector>
#include <VkBootstrap.h>
#include <vulkan/vulkan_core.h>
#include "Exception.hpp"
#include "Log.hpp"
#include "Types.hpp"
#include "Version.hpp"
#include "render/Window.hpp"
#include "vulkan-Check.hpp"

#ifdef R3_DEBUG
#define R3_VALIDATION_LAYERS_ENABLED 1
#else
#define R3_VALIDATION_LAYERS_ENABLED 0
#endif

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
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
            LOG_INFO("{}", pCallbackData->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
            LOG_WARNING("{}", pCallbackData->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
            LOG_ERROR("{}", pCallbackData->pMessage);
            break;
        default:
            break;
    }

    return VK_FALSE;
}
#endif

namespace R3 {

void RenderContext::create(Window& window) noexcept(false) {
    std::error_code error;

    try {
        const vkb::Instance instance             = createInstance();
        m_instance                               = instance.instance;
        m_debug                                  = instance.debug_messenger;
        m_surface                                = createSurface(window);
        const vkb::PhysicalDevice physicalDevice = selectPhysicalDevice(instance);
        m_physicalDevice                         = physicalDevice.physical_device;
        const vkb::Device device                 = createLogicalDevice(physicalDevice);
        m_logicalDevice                          = device.device;

        /* get queue handles */
        auto initQueue = [&](vkb::QueueType queueType, Queue& queue) {
            queue.type = queueType;

            if (auto result = device.get_queue(queueType); !result) {
                throw Exception{std::format("failed to get VkQueue: {}", result.error().value())};
            } else {
                queue.handle = result.value();
            }

            if (auto result = device.get_queue_index(queueType); !result) {
                throw Exception{std::format("failed to get VkQueue index: {}", result.error().value())};
            } else {
                queue.index = result.value();
            }
        };

        initQueue(vkb::QueueType::graphics, m_graphicsQueue);
        initQueue(vkb::QueueType::present, m_presentQueue);
        initQueue(vkb::QueueType::compute, m_computeQueue);
    } catch (const Exception& ex) {
        destroy();
        throw ex;
    }
} // namespace R3

void RenderContext::destroy() noexcept(true) {
    vkDestroyDevice(m_logicalDevice, nullptr);
    if (m_instance != VK_NULL_HANDLE) {
        vkDestroySurfaceKHR(m_instance, m_surface, nullptr);
        vkb::destroy_debug_utils_messenger(m_instance, m_debug, nullptr);
    }
    vkDestroyInstance(m_instance, nullptr);
}

vkb::Instance RenderContext::createInstance() const noexcept(false) {
    /* get instance extensions required by glfw */
    uint32 extensionCount;
    const char** pRequiredExtensions = glfwGetRequiredInstanceExtensions(&extensionCount);
    if (pRequiredExtensions == nullptr) {
        throw Exception("glfwGetRequiredInstanceExtensions returned null");
    }
    std::vector<const char*> requiredExtensions{pRequiredExtensions, pRequiredExtensions + extensionCount};

    /* add debug */
#if R3_VALIDATION_LAYERS_ENABLED
    requiredExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif

    auto result = vkb::InstanceBuilder()
                      .set_app_name("R3 Application")
                      .set_app_version(R3_VERSION_MAJOR, R3_VERSION_MINOR, R3_VERSION_PATCH)
                      .set_engine_name("R3 Engine")
                      .set_engine_version(R3_VERSION_MAJOR, R3_VERSION_MINOR, R3_VERSION_PATCH)
                      .require_api_version(R3_VULKAN_VERSION_MAJOR, R3_VULKAN_VERSION_MINOR, R3_VULKAN_VERSION_PATCH)
                      .request_validation_layers(R3_VALIDATION_LAYERS_ENABLED)
                      .enable_extensions(requiredExtensions)
                      .add_debug_messenger_severity(VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT)
                      .add_debug_messenger_severity(VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT)
                      .add_debug_messenger_severity(VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
                      .add_debug_messenger_severity(VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
                      .add_debug_messenger_type(VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT)
                      .add_debug_messenger_type(VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT)
                      .add_debug_messenger_type(VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT)
                      .set_debug_callback(validationDebugCallback)
                      .build();

    if (!result) {
        throw Exception{std::format("failed to create VkInstance: {}", result.error().value())};
    }

    return result.value();
}

VkSurfaceKHR RenderContext::createSurface(Window& window) const noexcept(false) {
    VkSurfaceKHR surface;
    VK_CHECK(glfwCreateWindowSurface(m_instance, window.glfw(), nullptr, &surface));
    return surface;
}

vkb::PhysicalDevice RenderContext::selectPhysicalDevice(const vkb::Instance& instance) const noexcept(false) {
    auto result = vkb::PhysicalDeviceSelector(instance)
                      .set_minimum_version(1, 3)
                      .set_required_features({
                          .geometryShader     = VK_TRUE,
                          .tessellationShader = VK_TRUE,
                          .fillModeNonSolid   = VK_TRUE,
                          .samplerAnisotropy  = VK_TRUE,
                      })
                      .set_required_features_12({
                          .descriptorIndexing  = VK_TRUE,
                          .bufferDeviceAddress = VK_TRUE,
                      })
                      .set_required_features_13({
                          .synchronization2 = VK_TRUE,
                          .dynamicRendering = VK_TRUE,
                      })
                      .add_required_extension(VK_KHR_SWAPCHAIN_EXTENSION_NAME)
                      .require_dedicated_transfer_queue()
                      .require_separate_compute_queue()
                      .prefer_gpu_device_type(vkb::PreferredDeviceType::discrete)
                      .set_surface(m_surface)
                      .select();

    if (!result) {
        throw Exception{std::format("failed to select VkPhysicalDevice: {}", result.error().value())};
    }

    return result.value();
}

vkb::Device RenderContext::createLogicalDevice(const vkb::PhysicalDevice& physicalDevice) const noexcept(false) {
    auto result = vkb::DeviceBuilder(physicalDevice).build();

    if (!result) {
        throw Exception{std::format("failed to create VkDevice: {}", result.error().value())};
    }

    return result.value();
}

} // namespace R3

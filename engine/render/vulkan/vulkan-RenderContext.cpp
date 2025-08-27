#include "vulkan-RenderContext.hpp"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <cstdint>
#include <format>
#include <iterator>
#include <span>
#include <system_error>
#include <type_traits>
#include <vector>
#include <VkBootstrap.h>
#include <vulkan/vulkan_core.h>
#include "api/Exception.hpp"
#include "api/Types.hpp"
#include "api/Version.hpp"
#include "core/Log.hpp"
#include "render/RenderContext.hpp"
#include "render/Window.hpp"
#include "vulkan-Check.hpp"
#include "vulkan-CommandBuffer.hpp"
#include "vulkan-Handle.hpp"

namespace R3::vulkan {

#if R3_VALIDATION_LAYERS_ENABLED

static VKAPI_ATTR VkBool32 VKAPI_CALL validationDebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                                              VkDebugUtilsMessageTypeFlagsEXT messageType,
                                                              const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                                                              void* pUserData);

#endif

RenderContext::RenderContext(Window& window)
    : IRenderContext(std::type_identity<decltype(*this)>()) {
    std::error_code error;

    try {
        const vkb::Instance instance             = createInstance();
        m_instance                               = instance.instance;
        m_debug                                  = instance.debug_messenger;
        m_surface                                = createSurface(window, m_instance);
        const vkb::PhysicalDevice physicalDevice = selectPhysicalDevice(instance, m_surface);
        m_physicalDevice                         = physicalDevice.physical_device;
        const vkb::Device device                 = createLogicalDevice(physicalDevice);
        m_device                                 = device.device;

        setupQueue(device, vkb::QueueType::graphics, m_graphicsQueue, m_graphicsQueueIndex);
        setupQueue(device, vkb::QueueType::present, m_presentQueue, m_presentQueueIndex);
        setupQueue(device, vkb::QueueType::compute, m_computeQueue, m_computeQueueIndex);

        createCommandPools();
        createSyncObjects();
        createDescritorSetLayouts();
    } catch (const Exception& ex) {
        this->~RenderContext();
        throw ex;
    }
}

RenderContext::~RenderContext() noexcept {
    if (m_device) {
        vkDeviceWaitIdle(m_device);

        vkDestroyDescriptorSetLayout(m_device, m_defaultDescriptorSetLayout, nullptr);

        for (auto& sem : m_imageAvailableSemaphores) {
            vkDestroySemaphore(m_device, sem, nullptr);
        }
        m_imageAvailableSemaphores.clear();

        for (auto& fence : m_inFlightFences) {
            vkDestroyFence(m_device, fence, nullptr);
        }
        m_inFlightFences.clear();

        for (auto& sem : m_renderFinishedSemaphores) {
            vkDestroySemaphore(m_device, sem, nullptr);
        }
        m_renderFinishedSemaphores.clear();

        m_graphicsQueueCmds.clear();
        m_computeQueueCmds.clear();

        // destroy device
        vkDestroyDevice(m_device, nullptr);
    }

    if (m_instance) {
        // destroy surface
        vkDestroySurfaceKHR(m_instance, m_surface, nullptr);

        // destroy debug messenger
        vkb::destroy_debug_utils_messenger(m_instance, m_debug, nullptr);

        // destroy instance
        vkDestroyInstance(m_instance, nullptr);
    }
}

void RenderContext::waitIdle() {
    VK_CHECK(vkDeviceWaitIdle(m_device));
}

void RenderContext::waitForCurrentFrame() {
    VK_CHECK(vkWaitForFences(m_device, 1, &m_inFlightFences[m_currentFrame], VK_TRUE, UINT64_MAX));
    VK_CHECK(vkResetFences(m_device, 1, &m_inFlightFences[m_currentFrame]));
}

uint32 RenderContext::queryDeviceMemoryTypeIndex(uint32 typeFilter, VkMemoryPropertyFlags properties) const {
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(m_physicalDevice, &memProperties);

    for (uint32 i = 0; i < memProperties.memoryTypeCount; i++) {
        if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }

    throw Exception("Failed to find suitable memory type");
}

VkSampleCountFlagBits RenderContext::queryMaxUsableSampleCount() const noexcept {
    VkPhysicalDeviceProperties physicalDeviceProperties;
    vkGetPhysicalDeviceProperties(m_physicalDevice, &physicalDeviceProperties);

    const VkSampleCountFlags counts = physicalDeviceProperties.limits.framebufferColorSampleCounts &
                                      physicalDeviceProperties.limits.framebufferDepthSampleCounts;

    return (counts & VK_SAMPLE_COUNT_64_BIT)   ? VK_SAMPLE_COUNT_64_BIT
           : (counts & VK_SAMPLE_COUNT_32_BIT) ? VK_SAMPLE_COUNT_32_BIT
           : (counts & VK_SAMPLE_COUNT_16_BIT) ? VK_SAMPLE_COUNT_16_BIT
           : (counts & VK_SAMPLE_COUNT_8_BIT)  ? VK_SAMPLE_COUNT_8_BIT
           : (counts & VK_SAMPLE_COUNT_4_BIT)  ? VK_SAMPLE_COUNT_4_BIT
           : (counts & VK_SAMPLE_COUNT_2_BIT)  ? VK_SAMPLE_COUNT_2_BIT
                                               : VK_SAMPLE_COUNT_1_BIT;
}

VkFormat RenderContext::queryDepthFormat() const noexcept {
    static constexpr VkFormat formats[] = {
        VK_FORMAT_D32_SFLOAT,
        VK_FORMAT_D32_SFLOAT_S8_UINT,
        VK_FORMAT_D24_UNORM_S8_UINT,
    };
    return querySupportedFormat(formats, VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
}

VkFormat RenderContext::querySupportedFormat(std::span<const VkFormat> formats,
                                             VkImageTiling tiling,
                                             VkFormatFeatureFlags features) const noexcept {
    for (VkFormat format : formats) {
        VkFormatProperties formatProperties;
        vkGetPhysicalDeviceFormatProperties(m_physicalDevice, format, &formatProperties);

        switch (tiling) {
            case VK_IMAGE_TILING_LINEAR:
                if ((formatProperties.linearTilingFeatures & features) == features) {
                    return format;
                }
                break;
            case VK_IMAGE_TILING_OPTIMAL:
                if ((formatProperties.optimalTilingFeatures & features) == features) {
                    return format;
                }
                break;
            default:
                continue;
        }
    }

    return VK_FORMAT_UNDEFINED;
}

vkb::Instance RenderContext::createInstance() {
    /* get instance extensions required by glfw */
    uint32 extensionCount;
    const char** pRequiredExtensions = glfwGetRequiredInstanceExtensions(&extensionCount);
    if (pRequiredExtensions == nullptr) {
        throw Exception("glfwGetRequiredInstanceExtensions returned null");
    }
    std::vector<const char*> requiredExtensions{pRequiredExtensions, pRequiredExtensions + extensionCount};
#if R3_VALIDATION_LAYERS_ENABLED
    requiredExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif

    auto result = vkb::InstanceBuilder()
                      .set_app_name("R3 Application")
                      .set_app_version(R3_ENGINE_VERSION_MAJOR, R3_ENGINE_VERSION_MINOR, R3_ENGINE_VERSION_PATCH)
                      .set_engine_name("R3 Engine")
                      .set_engine_version(R3_ENGINE_VERSION_MAJOR, R3_ENGINE_VERSION_MINOR, R3_ENGINE_VERSION_PATCH)
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
        throw Exception{
            std::format("failed to create VkInstance: {} {}", result.error().message(), result.error().value())};
    }

    return result.value();
}

VkSurfaceKHR RenderContext::createSurface(Window& window, VkInstance instance) {
    VkSurfaceKHR surface;
    VK_CHECK(glfwCreateWindowSurface(instance, window.glfw(), nullptr, &surface));
    return surface;
}

vkb::PhysicalDevice RenderContext::selectPhysicalDevice(const vkb::Instance& instance, VkSurfaceKHR surface) {
    auto result = vkb::PhysicalDeviceSelector(instance, surface)
                      .set_minimum_version(1, 3)
                      .set_required_features({
                          .geometryShader     = VK_TRUE,
                          .tessellationShader = VK_TRUE,
                          .sampleRateShading  = VK_TRUE,
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
                      .add_required_extensions({
                          VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME,
                          VK_KHR_PIPELINE_LIBRARY_EXTENSION_NAME,
                          VK_EXT_GRAPHICS_PIPELINE_LIBRARY_EXTENSION_NAME,
                          VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME,
                      })
                      .add_required_extension(VK_KHR_SWAPCHAIN_EXTENSION_NAME)
                      .require_dedicated_transfer_queue()
                      .require_separate_compute_queue()
                      .prefer_gpu_device_type(vkb::PreferredDeviceType::discrete)
                      .select();

    if (!result) {
        throw Exception{
            std::format("failed to select VkPhysicalDevice: {} {}", result.error().message(), result.error().value())};
    }

    return result.value();
}

vkb::Device RenderContext::createLogicalDevice(const vkb::PhysicalDevice& physicalDevice) {
    auto result = vkb::DeviceBuilder(physicalDevice).build();

    if (!result) {
        throw Exception{
            std::format("failed to create VkDevice: {} {}", result.error().message(), result.error().value())};
    }

    return result.value();
}

void RenderContext::setupQueue(const vkb::Device& device, vkb::QueueType queueType, VkQueue& queue, uint32& index) {
    if (auto result = device.get_queue(queueType); !result) {
        throw Exception{std::format("failed to get VkQueue: {} {}", result.error().message(), result.error().value())};
    } else {
        queue = result.value();
    }

    if (auto result = device.get_queue_index(queueType); !result) {
        throw Exception{
            std::format("failed to get VkQueue index: {} {}", result.error().message(), result.error().value())};
    } else {
        index = result.value();
    }
}

void RenderContext::createCommandPools() {
    const VkCommandPoolCreateFlags poolFlags =
        VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT | VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
    m_graphicsQueueCmds = CommandBuffer::allocate(*this, graphicsQueueIndex(), poolFlags, maxFramesInFlight());
    m_computeQueueCmds  = CommandBuffer::allocate(*this, computeQueueIndex(), poolFlags, maxFramesInFlight());
}

void RenderContext::createSyncObjects() {
    const VkSemaphoreCreateInfo semaphoreInfo = {
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
    };

    const VkFenceCreateInfo fenceInfo = {
        .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
        .pNext = nullptr,
        .flags = VK_FENCE_CREATE_SIGNALED_BIT,
    };

    // create per-frame acquire semaphores

    m_imageAvailableSemaphores.resize(maxFramesInFlight());
    for (auto& sem : m_imageAvailableSemaphores) {
        VK_CHECK(vkCreateSemaphore(m_device, &semaphoreInfo, nullptr, &sem));
    }

    m_inFlightFences.resize(maxFramesInFlight());
    for (auto& fence : m_inFlightFences) {
        VK_CHECK(vkCreateFence(m_device, &fenceInfo, nullptr, &fence));
    }

    // create per-image render finished semaphores

    // anything more than 4 is not supported

    static constexpr usize MAX_USED_SWAPCHAIN_IMAGES = 4;

    m_renderFinishedSemaphores.resize(MAX_USED_SWAPCHAIN_IMAGES);
    for (auto& sem : m_renderFinishedSemaphores) {
        VK_CHECK(vkCreateSemaphore(m_device, &semaphoreInfo, nullptr, &sem));
    }
}

void RenderContext::createDescritorSetLayouts() {
    //--- Descriptor Layout
    const VkDescriptorSetLayoutBinding bindings[] = {
        // { binding, type, count, stage }

        // Uniform Buffer Object
        {0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT},
        // Albedo
        {1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT},
        // MetallicRoughness
        {2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT},
        // Normal
        {3, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT},
        // AmbientOcclusion
        {4, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT},
        // Emissive
        {5, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT},
        // Lighting
        {6, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_FRAGMENT_BIT},
    };

    VkDescriptorSetLayoutCreateInfo layoutInfo = {
        .sType        = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
        .pNext        = nullptr,
        .flags        = {},
        .bindingCount = static_cast<uint32>(std::size(bindings)),
        .pBindings    = bindings,
    };
    VK_CHECK(vkCreateDescriptorSetLayout(m_device, &layoutInfo, nullptr, &*m_defaultDescriptorSetLayout));
}

#if R3_VALIDATION_LAYERS_ENABLED

VKAPI_ATTR VkBool32 VKAPI_CALL validationDebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                                       VkDebugUtilsMessageTypeFlagsEXT messageType,
                                                       const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                                                       void* pUserData) {
    (void)messageType;
    (void)pCallbackData;
    (void)pUserData;

    switch (messageSeverity) {
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
            LOG_VERBOSE("{}", pCallbackData->pMessage);
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

} // namespace R3::vulkan

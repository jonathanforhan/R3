#if R3_VULKAN

#pragma once
#include <vulkan/vulkan.h>
#include <span>

namespace R3::vulkan {

class VulkanRenderer {
public:
    VulkanRenderer();
    ~VulkanRenderer();

private:
    void createInstance();

    void choosePhysicalDevice();

    void setupDebugMessenger();

    bool checkInstanceExtensionSupport(std::span<const char*> requiredExtensions) const;
    bool checkValidationLayerSupport() const;
    bool checkPhysicalDeviceSuitable(VkPhysicalDevice physicalDevice) const;

private:
    VkInstance m_instance;
    VkPhysicalDevice m_physicalDevice;

#if !R3_BUILD_DIST
    VkDebugUtilsMessengerEXT m_debugMessenger;

    static constexpr const char* VALIDATION_LAYERS[]{
        "VK_LAYER_KHRONOS_validation",
    };
#endif // !R3_BUILD_DIST
};

} // namespace R3::vulkan

#endif // R3_VULKAN